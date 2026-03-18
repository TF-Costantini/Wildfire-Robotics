# Exports the path to remoteocd so that it can be called as 'remoteocd' without the full exe path
# Creates REMOTEOCD_CFG env variable to simplify call commands
# Exports the path to openocd so that it can be called as 'openocd' without the full exe path

{
echo "export PATH=$PATH:/home/arduino/.arduino15/packages/arduino/tools/remoteocd/0.0.4-rc.4"
echo "export REMOTEOCD_CFG=/home/arduino/.arduino15/packages/arduino/hardware/zephyr/0.53.1/variants/arduino_uno_q_stm32u585xx/flash_bootloader.cfg"
echo "export PATH=$PATH:/home/arduino/.arduino15/packages/SiliconLabs/tools/openocd/0.12.0-arduino1-static/bin"
} >> home/arduino/.bashrc

# Reloads sources to apply the new environment
source /home/arduino/.bashrc