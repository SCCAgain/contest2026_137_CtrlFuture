# STM32N647X0 Hardware Bringup Checklist

## Prerequisites

- [ ] ST-Link V3 debugger connected (SWD: SWDIO, SWCLK, GND, 3.3V)
- [ ] USB-to-serial adapter on USART1 (PE5=TX, PE6=RX, 115200 8N1)
- [ ] STM32CubeProgrammer CLI installed
- [ ] DEV boot mode selected (BOOT0=1, BOOT1=0)
- [ ] SD card inserted (FAT32 formatted)
- [ ] Ethernet cable connected

## Phase 0: Basic Boot (Day 1)

```bash
# Build
cd ~/openvela
./build.sh vendor/openvela/boards/contest2026_137_board/configs/nsh -j8

# Flash via ST-Link (DEV boot, SRAM load)
STM32_Programmer_CLI -c port=SWD freq=8000 \
  -w nuttx/nuttx.bin 0x34000400 -v -rst

# Connect serial
minicom -D /dev/ttyACM0 -b 115200
```

- [ ] NSH shell prompt appears
- [ ] `help` command works
- [ ] `ps` shows running tasks
- [ ] `hello` builtin app runs

## Phase 1: Clock and Basic Peripherals (Week 1)

- [ ] Enable 800MHz clock (CONFIG_EDGESIGHT_CLOCK_800MHZ)
- [ ] Verify with `coremark` benchmark
- [ ] GPIO LED blink test
- [ ] USART1 console stable at 115200

## Phase 2: Storage (Week 1-2)

- [ ] SDMMC1 init (SD card detected)
- [ ] FatFS mount at /mnt/sd
- [ ] File read/write test
- [ ] XSPI Flash memory-mapped mode

## Phase 3: Network (Week 2)

- [ ] Ethernet PHY init (LAN8742 link up)
- [ ] lwIP DHCP address assigned
- [ ] ping external host
- [ ] MQTT connect to broker

## Phase 4: Display (Week 2-3)

- [ ] LTDC init (800x480)
- [ ] Test pattern on LCD
- [ ] Double-buffer swap
- [ ] DCMIPP camera init
- [ ] Camera image on LCD

## Phase 5: AI (Week 3-4)

- [ ] NPU SRAM enable
- [ ] stedgeai model load
- [ ] YOLO inference (person detection)
- [ ] MoveNet inference (pose estimation)
- [ ] Fall detection demo

## Phase 6: Full Integration (Week 4-5)

- [ ] EdgeSight app start
- [ ] Camera -> NPU -> Display pipeline
- [ ] Event recording to SD card
- [ ] MQTT alert on fall
- [ ] NSH command interface
- [ ] Performance profiling
