# P2Button OTA API Specification — v1.0

**Status**: Locked 2026-06-08. Owner: MoSaleh delivers spec; Ali implements firmware.
**Scope**: BLE wire format + firmware module responsibilities for OTA + remote logging + anti-theft.
**Plan reference**: `~/.claude/plans/lets-make-the-studio-expressive-cook.md` v6 SUPER PLAN.

## Module ownership

Ali owns 4 firmware modules in `Hardware/P2Button/firmware/`:

1. `ota.cpp` — FIRMWARE_UPDATE_SERVICE BLE + esp_ota_*
2. `logger.cpp` — SD_MMC ring buffer + LOGS_SERVICE BLE
3. `boot_validation.cpp` — 30s health check + auto-rollback
4. `anti_theft.cpp` — Per-device ECDSA P-256 keypair + ANTI_THEFT_SERVICE BLE

MoSaleh provides the embedded public key header (`firmware/include/signing_pubkey.h`) after Phase A.2.

---

## BLE Service UUIDs

All UUIDs use the Pindar prefix `6e4f` for easy bench identification.

```
FIRMWARE_UPDATE_SERVICE     6e4f0001-b5a3-f393-e0a9-e50e24dcca9e
  CHAR_CMD                  6e4f0002-b5a3-f393-e0a9-e50e24dcca9e  WRITE+RESPONSE
  CHAR_CHUNK                6e4f0003-b5a3-f393-e0a9-e50e24dcca9e  WRITE w/o RESPONSE
  CHAR_STATUS               6e4f0004-b5a3-f393-e0a9-e50e24dcca9e  NOTIFY

LOGS_SERVICE                6e4f0005-b5a3-f393-e0a9-e50e24dcca9e
  CHAR_LOG_REQUEST          6e4f0006-b5a3-f393-e0a9-e50e24dcca9e  WRITE+RESPONSE
  CHAR_LOG_CHUNK            6e4f0007-b5a3-f393-e0a9-e50e24dcca9e  NOTIFY
  CHAR_LOG_DONE             6e4f0008-b5a3-f393-e0a9-e50e24dcca9e  NOTIFY

ANTI_THEFT_SERVICE          6e4f0009-b5a3-f393-e0a9-e50e24dcca9e
  CHAR_CHALLENGE            6e4f000a-b5a3-f393-e0a9-e50e24dcca9e  WRITE+RESPONSE
  CHAR_DEVICE_INFO          6e4f000b-b5a3-f393-e0a9-e50e24dcca9e  READ
  CHAR_LOCK                 6e4f000c-b5a3-f393-e0a9-e50e24dcca9e  WRITE+RESPONSE
```

---

## FIRMWARE_UPDATE_SERVICE — wire format

### CHAR_CMD (write w/ response) — phone sends commands

JSON UTF-8 encoded.

**BEGIN**:
```json
{
  "cmd": "BEGIN",
  "version": "1.0.2",
  "targetModel": "p2button-v3",
  "sha256": "ab12...64hex",
  "sigBase64": "MEUCIQ...base64",
  "totalBytes": 524288,
  "chunkSize": 244
}
```

Device action:
1. ECDSA P-256 verify `sigBase64` over `(sha256 || totalBytes || version)` using embedded public key
2. If verify fails → notify STATUS with `{"phase":"ERROR","error":"INVALID_SIGNATURE"}` + abort
3. If `version <= currentVersion` (semver compare, NOT alphabetic) → notify STATUS `VERSION_OLDER` + abort
4. If `targetModel != device.model` → notify STATUS `MODEL_MISMATCH` + abort
5. `esp_ota_begin(esp_ota_get_next_update_partition(NULL), totalBytes, &ota_handle)`
6. Reset `bytesReceived = 0`, `chunkIndex = 0`
7. Notify STATUS `{"phase":"RECEIVING","bytesReceived":0}`

**COMMIT**:
```json
{ "cmd": "COMMIT" }
```

Device action:
1. Compute SHA-256 of accumulated bytes
2. If `computedHash != metadata.sha256` → notify STATUS `HASH_MISMATCH` + `esp_ota_abort`
3. `esp_ota_end(ota_handle)`
4. `esp_ota_set_boot_partition(update_partition)`
5. Notify STATUS `{"phase":"REBOOTING"}`
6. Delay 500ms; `esp_restart()`

**ABORT**:
```json
{ "cmd": "ABORT" }
```

Device action: `esp_ota_abort(ota_handle)` + reset state + notify STATUS `{"phase":"ABORTED"}`.

### CHAR_CHUNK (write w/o response) — phone sends bytes

Raw bytes only. Each write is one chunk.

**Wire format**: `[uint16_BE chunk_index][payload_bytes]`
- `chunk_index`: 0-based; must increment by 1 each chunk
- `payload_bytes`: up to (MTU - 5) bytes (3-byte ATT header + 2-byte chunk_index)

iOS MTU caps at ~185 → payload ≤180 bytes per chunk.
Android MTU up to 517 → payload ≤512 bytes per chunk.

Phone negotiates actual MTU via `requestMTU()` at OTA start; device echoes effective MTU in STATUS `{"phase":"RECEIVING","effectiveMtu":N}` after BEGIN.

Device action:
1. If `chunk_index != expected_next_chunk` → notify STATUS `{"phase":"ERROR","error":"OUT_OF_ORDER","expected":N}` + abort
2. `esp_ota_write(ota_handle, payload_bytes, length)`
3. `bytesReceived += length; expected_next_chunk++`
4. Every 32 chunks: notify STATUS `{"phase":"RECEIVING","bytesReceived":N}` (flow control hint)

### CHAR_STATUS (notify) — device → phone

```json
{
  "phase": "RECEIVING" | "VERIFYING" | "FLASHING" | "REBOOTING" | "ABORTED" | "ERROR",
  "bytesReceived": 12345,
  "effectiveMtu": 244,
  "error": "INVALID_SIGNATURE" | "HASH_MISMATCH" | "VERSION_OLDER" | "MODEL_MISMATCH" | "OUT_OF_ORDER" | "OTA_WRITE_FAILED" | "STORAGE_FULL"
}
```

Phone subscribes to STATUS before sending BEGIN.

---

## LOGS_SERVICE — wire format

### SD card log file layout

Path: `/logs/YYYYMMDD.csv` (one file per UTC day).
Encoding: UTF-8, no BOM, LF line endings.
Format:
```
timestamp,event_type,payload
2026-06-08T15:30:42.123Z,BLE_CONNECT,user_id=42
2026-06-08T15:30:43.001Z,BUTTON_PRESS,duration_ms=350
2026-06-08T15:30:44.500Z,STATE_CHANGE,from=LOCKED to=UNLOCKED
```

**Daily rotation**: at 00:00 UTC, start new file.
**Retention**: on boot, delete files where YYYYMMDD < (today - 60 days).
**Storage overflow**: if SD card full, drop OLDEST file + log `SD_FULL` event to current file. Never block other operations.

### CHAR_LOG_REQUEST (write w/ response)

```json
{ "rangeDays": 60 }
```

Device action:
1. Enumerate `/logs/*.csv` where date >= (today - rangeDays)
2. Concatenate in chronological order
3. Stream via CHAR_LOG_CHUNK notifications
4. Final CHAR_LOG_DONE notification

### CHAR_LOG_CHUNK (notify)

Raw bytes. Each notification:
**Wire format**: `[uint16_BE chunk_index][csv_bytes]`

Up to (MTU - 5) bytes payload. Phone assembles into single CSV buffer.

### CHAR_LOG_DONE (notify)

```json
{ "totalChunks": 47, "totalBytes": 8932 }
```

Phone validates `received_chunks == totalChunks` + `received_bytes == totalBytes`. If mismatch, log a warning to backend but still upload partial CSV.

---

## ANTI_THEFT_SERVICE — wire format

### CHAR_DEVICE_INFO (read)

Device returns:
```json
{
  "mac": "AA:BB:CC:DD:EE:FF",
  "publicKeyPem": "-----BEGIN PUBLIC KEY-----\nMFkwEwYH...\n-----END PUBLIC KEY-----",
  "fwVersion": "1.0.2",
  "model": "p2button-v3",
  "monotonicCounter": 42,
  "needsPubkeyUpload": false
}
```

`needsPubkeyUpload=true` if device generated keypair on this boot and hasn't yet been seen by server (TOFU upload pending).

### CHAR_CHALLENGE (write w/ response)

Phone sends 32-byte random nonce.

Device action:
1. Compute `msg = nonce || monotonicCounter` (counter as uint32_BE)
2. Sign with device private key (ECDSA P-256, mbedTLS software)
3. Encode signature IEEE P1363 r||s (64 bytes), base64
4. Increment monotonicCounter, persist to NVS
5. Return:
```json
{
  "monotonicCounter": 43,
  "signature": "base64..."
}
```

Server receives via phone proxy at `POST /api/buttons/:mac/challenge` and:
1. Verifies signature with stored public_key_pem
2. Asserts `monotonicCounter > stored.monotonic_counter` (strictly greater)
3. UPDATE buttons SET monotonic_counter = new_counter, last_challenge_at = NOW()

### CHAR_LOCK (write w/ response)

Server sends signed lock command:
```json
{
  "cmd": "LOCK" | "UNLOCK",
  "issuedAt": "2026-06-08T15:30:00Z",
  "sigBase64": "..."
}
```

`sigBase64` is ECDSA signature of `(cmd || issuedAt || mac)` using FIRMWARE signing key (same key that signs firmware bundles).

Device action:
1. Verify signature with embedded public key
2. If verify fails → notify STATUS `LOCK_SIGNATURE_INVALID`, ignore
3. If cmd=LOCK: disable button, blank OLED to "DEVICE LOCKED — CONTACT PINDAR", red LED solid, mute buzzer
4. If cmd=UNLOCK: restore normal operation

---

## Signature formats

**ECDSA P-256** (firmware signing + lock commands):
- Hash: SHA-256
- Curve: secp256r1 / prime256v1
- Signature: IEEE P1363 raw r||s, 64 bytes total
- Encoding: base64 standard (with padding)

**ECDSA P-256** (per-device challenge response):
- Same curve + format
- Device generates keypair once on first boot
- Private key stored in eFuse (if capacity) OR encrypted NVS partition

---

## Boot validation criteria

Device firmware calls `esp_ota_mark_app_valid_cancel_rollback()` within 30 seconds of boot **only if all of these are true**:

1. `BLEDevice::init()` succeeded AND advertising started
2. Button GPIO ISR registered AND single hardware press detected (auto-test) OR 5s passes without crash
3. OLED `display.begin()` succeeded AND a single render frame completed without exception
4. SD_MMC.begin() succeeded (logger module ready)

If any fail within 30s → return without calling mark_valid → bootloader auto-rollback on next reboot (ESP-IDF built-in behavior).

**Boot loop guard** (audit Risk #1): if 3+ consecutive rollbacks detected in /logs/, set NVS flag `boot_loop=1` and refuse OTA updates until cleared manually via reset button + BLE recovery command (defer recovery flow to v1.1).

---

## Per-device keypair generation

On first boot (NVS key `device_priv_key` is empty):

```cpp
mbedtls_ecdsa_context ctx;
mbedtls_ecdsa_init(&ctx);
mbedtls_ecp_group_load(&ctx.grp, MBEDTLS_ECP_DP_SECP256R1);
mbedtls_ecdsa_genkey(&ctx, MBEDTLS_ECP_DP_SECP256R1, my_rand, NULL);

// Serialize private key (raw 32 bytes)
unsigned char priv_buf[32];
mbedtls_mpi_write_binary(&ctx.d, priv_buf, 32);

// Store in eFuse if available, else encrypted NVS
nvs_set_blob(handle, "device_priv_key", priv_buf, 32);
nvs_commit(handle);

// Serialize public key as PEM
unsigned char pub_pem[512];
mbedtls_pk_write_pubkey_pem(&pk_ctx, pub_pem, sizeof(pub_pem));

// Store + flag for TOFU upload
nvs_set_str(handle, "device_pub_pem", (char*)pub_pem);
nvs_set_u8(handle, "needs_pubkey_upload", 1);
```

On next heartbeat, phone reads CHAR_DEVICE_INFO, sees `needsPubkeyUpload=true`, POSTs the public key to backend. Backend stores in `buttons.public_key_pem`. Device clears the flag on next heartbeat after seeing the public key persisted server-side.

**TOFU risk**: an attacker intercepting the first heartbeat could substitute their own public key. Acknowledged risk for v1.0 (5 deployed buttons in trusted Pindar venues). v1.1 mitigation: factory-burn keypair before deployment + ship public key with device sticker QR.

---

## RAM + flash budget (ESP32-S3, Seeed XIAO)

| Component | Estimated RAM | Estimated flash |
|---|---|---|
| Existing firmware (P2Button.ino + libs) | ~120 KB | ~600 KB |
| NimBLE +3 new services | ~20 KB | ~30 KB |
| esp_ota_write buffers | ~30 KB | 0 |
| mbedTLS ECDSA P-256 | ~10 KB | ~50 KB |
| SD_MMC + FATFS | ~20 KB | ~80 KB |
| Logger ring buffer | ~5 KB | minimal |
| **Total** | **~205 KB / 320 KB SRAM** | **~770 KB / 4 MB app partition** |

PSRAM (8 MB) available for temporary buffers if needed. Total well within budget.

---

## Sign-off

- **API contract**: MoSaleh — 2026-06-08
- **Ali implementation start**: pending PC341 merge status check (T22.0 Day 0)
- **First integration test**: T22.20 — Day 19 (Jul 3)
