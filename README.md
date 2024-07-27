# IOS-Il2CppDumper

IOS Il2CppDumper, dump il2cpp data at runtime, can bypass protection, encryption and obfuscation.

## How to Us

- Install Theos (https://theos.dev/) and iPhoneOS14.4.sdk
- Clone the repository:
  
  ```bash
  
  git clone https://github.com/tien0246/IOS-Il2CppDumper.git

- Change bundle id in **IOSIl2CppDumper.plist**
- Run the game and follow the popups!

### Outputs

#### Dump.txt

All the dumped data. The file is written in C# format; you can use [Sublime Text](https://www.sublimetext.com) to view it with syntax highlighting.

#### Assembly folder

Dumped data by each DLL.

#### Script.json

For ida.py

#### ida.py

For IDA

### Configuration

All the configuration options are located in scr/Core/config.h

Available options:

* `BINARY_NAME`
  * Name of binary like `UnityFramework`

* `DUMP_FOLDER`
  * Name folder output

* `WAIT_TIME_SEC`
  * Waiting time before dump

* `DEBUG`
  * Output logs.txt for debug

* `GENSCRIPT`
  * Generate scrpit.json

## Disclaimer

Made for educational purpose. 
Not every game can be dumped with this tool, if it doesn't work in certain games it needs a bit of manual work. 

## Credits & Thanks

- [Tien0246](https://github.com/tien0246)
- [Batchhh](https://github.com/Batchhh)
- [Il2CppDumper](https://github.com/Perfare/Il2CppDumper)
- [UE4Dumper-4.25](https://github.com/guttir14/UnrealDumper-4.25)
