# FreeRTOS v10.3.1 on ARM fixed virtual platform FVP_Base_RevC-2xAEMv8
FreeRTOS port for ARM FVP (Fixed Virtual Platform) AEM_Arvm8-A: FVP_Base_RevC-2xAEMv8. The virtual platform is an Armv8-A model with GIC (Generic Interrupt Controller) supported.

This port can be adapted for any Armv8-A platforms.

Differences from the port GCC\ARM_CA53_64_BIT:
- A virtual platform is used for demonstration
- The maximum number of unique task priorities is changed from 32 to 64 (Armv8-A aarch64 platform)
- System registers are used for GIC CPU interfaces.
- CMake scripts used for building. The armclang compiler is used.
- Removed interrupt nesting support

# Build and run the demo
Download the ARM FVP from [here](https://developer.arm.com/tools-and-software/simulation-models/fixed-virtual-platforms) and install it on your machine.
Follow the steps below to build:
1. Create a build folder at Demo/FVP_Base_RevC-2xAEMv8A/, e.g. mkdir cmk_build
2. CD to cmk_build
3. Run "cmake ../" to generate the build scripts
4. Build by running "cmake --build . -v -j8"
5. Check out the binary freertosDemo.axf in cmk_build folder.

The configuration parameters for the FVP:
```
-C bp.secure_memory=false -C bp.terminal_0.mode=raw -C bp.refcounter.non_arch_start_at_default=1 -C cluster0.gicv3.FIQEn-RAO=1
```


