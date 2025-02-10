let device = null;
const SERVICES = [
  "19b10000-e8f2-537e-4f6c-d104768a1214",
  "19b20000-e8f2-537e-4f6c-d104768a1214",
]; // List of service UUIDs
const COMMANDS = [
  {
    serviceUUID: "19b10000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b10001-e8f2-537e-4f6c-d104768a1214",
    name: "Unlock (Button)",
    canRead: false,
    canWrite: false,
    canIndicate: true,
    canNotify: false,
  },
  {
    serviceUUID: "19b10000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b10002-e8f2-537e-4f6c-d104768a1214",
    name: "Unlock",
    canRead: false,
    canWrite: true,
    canIndicate: false,
    canNotify: false,
  },
  {
    serviceUUID: "19b10000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b10003-e8f2-537e-4f6c-d104768a1214",
    name: "Record (Button)",
    canRead: false,
    canWrite: false,
    canIndicate: true,
    canNotify: false,
  },
  {
    serviceUUID: "19b10000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b10004-e8f2-537e-4f6c-d104768a1214",
    name: "Record",
    canRead: false,
    canWrite: true,
    canIndicate: false,
    canNotify: false,
  },
  {
    serviceUUID: "19b10000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b10005-e8f2-537e-4f6c-d104768a1214",
    name: "Upload (Button)",
    canRead: false,
    canWrite: false,
    canIndicate: true,
    canNotify: false,
  },
  {
    serviceUUID: "19b20000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b20001-e8f2-537e-4f6c-d104768a1214",
    name: "Upload",
    canRead: false,
    canWrite: true,
    canIndicate: false,
    canNotify: false,
  },
  {
    serviceUUID: "19b20000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b20003-e8f2-537e-4f6c-d104768a1214",
    name: "Login",
    canRead: false,
    canWrite: true,
    canIndicate: false,
    canNotify: false,
  },
  {
    serviceUUID: "19b20000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b20004-e8f2-537e-4f6c-d104768a1214",
    name: "Logout",
    canRead: false,
    canWrite: true,
    canIndicate: false,
    canNotify: false,
  },
  {
    serviceUUID: "19b20000-e8f2-537e-4f6c-d104768a1214",
    uuid: "19b20005-e8f2-537e-4f6c-d104768a1214",
    name: "Sleep",
    canRead: false,
    canWrite: true,
    canIndicate: false,
    canNotify: false,
  },
];

const log = (message) => {
  const logDiv = document.getElementById("logs");
  const logMessage = document.createElement("p");
  logMessage.textContent = message;
  logDiv.appendChild(logMessage);
  logDiv.scrollTop = logDiv.scrollHeight; // Auto-scroll to the latest log
};

document.getElementById("connect").addEventListener("click", async () => {
  try {
    log("Requesting BLE device...");
    device = await navigator.bluetooth.requestDevice({
      acceptAllDevices: true,
      optionalServices: SERVICES,
    });

    log("Connecting to GATT server...");
    const server = await device.gatt.connect();

    log("Connected to BLE device!");
    initializeCharacteristics();
  } catch (error) {
    log(`Error: ${error.message}`);
  }
});

const sendCommand = async (serviceUUID, uuid, command) => {
  try {
    const service = await device.gatt.getPrimaryService(serviceUUID);
    const characteristic = await service.getCharacteristic(uuid);

    const encoder = new TextEncoder();
    await characteristic.writeValue(encoder.encode(command));
    log(`Command written: ${command}`);
  } catch (error) {
    log(`Error writing command: ${error.message}`);
  }
};

const subscribeToIndications = async (serviceUUID, uuid) => {
  try {
      const service = await device.gatt.getPrimaryService(serviceUUID);
      const characteristic = await service.getCharacteristic(uuid);

      await characteristic.startNotifications();
      log(`Subscribed to indications for ${uuid}`);

      characteristic.addEventListener("characteristicvaluechanged", (event) => {
          const value = new TextDecoder().decode(event.target.value);
          log(`Indication received from ${uuid}: ${value}`);
          console.log(`Raw value from ${uuid}:`, event.target.value);
      });
  } catch (error) {
      log(`Error subscribing to indications: ${error.message}`);
  }
};


const initializeCharacteristics = () => {
  const characteristicsContainer = document.getElementById("characteristicsContainer");
  characteristicsContainer.innerHTML = "";

  COMMANDS.forEach((command) => {
    const charDiv = document.createElement("div");
    charDiv.className = "mb-3";

    const charDetails = `
      <strong>${command.name}</strong> <br>
      <small>UUID: ${command.uuid}</small> <br>
      <small>Read: ${command.canRead ? "✔️" : "❌"}, Write: ${command.canWrite ? "✔️" : "❌"}, 
      Indicate: ${command.canIndicate ? "✔️" : "❌"}, Notify: ${command.canNotify ? "✔️" : "❌"}</small>
    `;

    charDiv.innerHTML = charDetails;

    if (command.canWrite) {
      const writeButton = document.createElement("button");
      writeButton.className = "btn btn-success mt-2";
      writeButton.textContent = `Send ${command.name}`;
      writeButton.addEventListener("click", () => sendCommand(command.serviceUUID, command.uuid, command.name));
      charDiv.appendChild(writeButton);
    }

    if (command.canIndicate) {
      const indicateButton = document.createElement("button");
      indicateButton.className = "btn btn-warning mt-2";
      indicateButton.textContent = `Subscribe to ${command.name}`;
      indicateButton.addEventListener("click", () => subscribeToIndications(command.serviceUUID, command.uuid));
      charDiv.appendChild(indicateButton);
    }

    characteristicsContainer.appendChild(charDiv);
  });
};
