# Boards compatible with this app 
TARGET_BOARDS := pca10090 pca10153 nrf52832_mdk_v2 pca10059 wuerth_261101102 promistel_rpi_hat mdbt50q_rx pca10040 pca10056 pca10100 ublox_b204 silabs_brd4254a tbsense2 silabs_brd4253a bgm220-ek4314a silabs_brd4184a silabs_brd4180b silabs_brd4312a silabs_brd4181b silabs_brd4210a silabs_brd2601b silabs_brd4187c silabs_brd2703a pan1780 

# Define a specific application area_id
app_specific_area_id=0x846B74

# App version
app_major=$(sdk_major)
app_minor=$(sdk_minor)
app_maintenance=$(sdk_maintenance)
app_development=$(sdk_development)

# Uncomment to allow reading scratchpad via dual-MCU API
#allow_scratchpad_read=yes
