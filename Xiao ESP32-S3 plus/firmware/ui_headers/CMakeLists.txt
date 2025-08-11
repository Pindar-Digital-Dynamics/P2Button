SET(SOURCES ui_LOGO1.c
    ui_LOGO2.c
    ui_LOGIN.c
    ui_HoldToUnlock.c
    ui_Unlock.c
    ui_Recording.c
    ui_HoldToUpload.c
    ui_Uploading.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_img_0_intro_png.c
    ui_img_1_logo_png.c
    ui_img_4_login_png.c
    ui_img_3a_unlock_png.c
    ui_img_3b_unlock_png.c
    ui_img_5_recording_png.c
    ui_img_1051634396.c
    ui_img_6_upload_png.c)

add_library(ui ${SOURCES})
