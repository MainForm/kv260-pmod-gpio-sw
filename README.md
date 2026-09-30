# KV260 PMOD GPIO Software

AMD Kria KV260에서 AXI GPIO 출력을 시험하기 위한 Vitis bare-metal 프로젝트입니다.
Vitis workspace 전체를 Git에 저장하는 대신, 하드웨어 XSA와 애플리케이션 소스 및
설정만 `src/`에서 관리하고 필요할 때 `scripts/checkout.py`로 workspace를
재생성합니다.

## 개발 환경

- AMD Vivado 2026.1
- AMD Vitis 2026.1
- Xilinx Kria KV260 Vision AI Starter Kit
- Zynq UltraScale+ MPSoC standalone application
- 대상 프로세서: `psu_cortexa53_0`

저장소에 포함된 `kv260_pmod_led.xsa`는 Vivado 2026.1로 생성되었습니다. XSA와
다른 버전의 Vitis를 조합하면 플랫폼 생성 또는 export 과정에서 문제가 발생할 수
있으므로 동일한 2026.1 버전 사용을 권장합니다.

## 애플리케이션 동작

`kv260_gpio_test`는 AXI GPIO 채널 1을 출력으로 설정하고 다음 값을 반복해서
출력합니다.

```text
GPIO High: 5 ms
GPIO Low : 5 ms
```

따라서 전체 주기는 10 ms이며 약 100 Hz로 GPIO 출력이 토글됩니다. 실행 시 UART에
다음 시작 메시지를 출력합니다.

```text
AXI GPIO LED Test Start
```

## 저장소 구성

```text
.
├── scripts/                         # checkin/checkout 도구 Git submodule
├── src/
│   ├── kv260_gpio_platform/
│   │   └── kv260_pmod_led.xsa      # Vivado hardware handoff
│   └── kv260_gpio_test/
│       ├── comp-settings.json      # Vitis 앱, 플랫폼, domain 설정
│       └── src/                    # 애플리케이션 소스와 링커 스크립트
└── ws/                              # 생성되는 Vitis workspace, Git 제외
```

`scripts/`는 다음 저장소의 `new_vitis/master` 브랜치를 추적하는 submodule입니다.

- [MainForm/digilent-vitis-scripts](https://github.com/MainForm/digilent-vitis-scripts)

## 저장소 받기

처음 clone할 때 submodule까지 함께 받습니다.

```powershell
git clone --recurse-submodules <repository-url>
cd kv260-pmod-gpio-sw
```

이미 clone한 저장소라면 다음 명령으로 submodule을 초기화합니다.

```powershell
git submodule update --init --recursive
```

## Vitis 실행 준비

Windows `PATH`에 다음 Vitis 실행 파일 경로를 추가하면 `vitis` 명령을 바로 사용할
수 있습니다.

```text
C:\AMDDesignTools\2026.1\Vitis\bin
```

환경변수를 추가한 뒤에는 실행 중인 VS Code와 터미널을 모두 종료하고 다시
실행해야 변경된 `PATH`가 적용됩니다.

```powershell
Get-Command vitis.bat
```

## Workspace 생성

Vitis가 실행 중이라면 먼저 종료한 후 저장소 최상위 폴더에서 실행합니다.

```powershell
vitis -s .\scripts\checkout.py
```

기본 checkout은 다음 컴포넌트를 `ws/`에 생성합니다.

- `kv260_pmod_led` platform component
- `standalone_psu_cortexa53_0` application domain
- ZynqMP FSBL domain
- `kv260_pmod_led_FSBL` application component
- `kv260_gpio_test` application component

Vitis는 application component 생성에 `.xpfm`이 필요하므로 기본 checkout에서도
플랫폼 export를 위한 최소 platform build는 수행합니다. FSBL과 사용자
애플리케이션 ELF 컴파일은 수행하지 않습니다.

기존 `ws/`를 확인 없이 삭제하고 다시 생성하려면 `-y`를 사용합니다.

```powershell
vitis -s .\scripts\checkout.py -y
```

> `-y`는 checkin하지 않은 workspace 변경 사항도 확인 없이 삭제합니다. 필요한
> 변경은 먼저 checkin해야 합니다.

## 빌드

### Vitis Unified IDE에서 빌드

기본 checkout 후 Vitis에서 `ws/`를 workspace로 열고 **Build All**을 실행합니다.
생성된 FSBL 프로젝트는 `build/fsbl.elf`을 만들며, 플랫폼은 이 경로를 사용해
디버거가 요구하는 다음 파일을 export합니다.

```text
ws/kv260_pmod_led/export/kv260_pmod_led/sw/boot/fsbl.elf
```

### Checkout과 함께 전체 빌드

workspace 생성부터 FSBL 연결, 플랫폼 최종 export, application ELF 빌드까지 한 번에
수행하려면 `--build`를 사용합니다.

```powershell
vitis -s .\scripts\checkout.py --build
```

기존 workspace를 확인 없이 다시 만들면서 전체 빌드하려면:

```powershell
vitis -s .\scripts\checkout.py -y --build
```

## Workspace 변경 사항 Check-in

Vitis에서 애플리케이션 소스나 설정을 변경한 경우 Vitis를 종료한 다음 실행합니다.

```powershell
vitis -s .\scripts\checkin.py
```

checkin은 다음 항목을 `src/`에 보존합니다.

- application source와 CMake/링커 설정
- application build options
- 사용한 XSA의 상대 경로
- 정확한 Vitis domain 이름, CPU instance와 OS

`ws/`의 BSP, object, ELF, export 결과와 IDE 메타데이터는 생성 파일이므로 Git으로
관리하지 않습니다.

## 디버깅 확인

Vitis에서 Build All을 완료한 뒤 다음 파일이 존재하는지 확인할 수 있습니다.

```powershell
Test-Path .\ws\kv260_pmod_led\export\kv260_pmod_led\sw\boot\fsbl.elf
Test-Path .\ws\kv260_gpio_test\build\kv260_gpio_test.elf
```

두 명령이 모두 `True`이면 Vitis debug configuration에서 사용할 FSBL과 application
ELF가 준비된 상태입니다.

## Scripts 문서

checkout/checkin 옵션과 스크립트 내부 동작에 관한 자세한 내용은
[scripts/README.md](scripts/README.md)를 참고하십시오.
