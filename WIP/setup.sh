{
# Exports the path to remoteocd so that it can be called as 'remoteocd' without the full exe path
echo "export PATH=$PATH:/home/arduino/.arduino15/packages/arduino/tools/remoteocd/0.0.4-rc.4"

# Creates REMOTEOCD_CFG env variable to simplify call commands
echo "export REMOTEOCD_CFG=/home/arduino/.arduino15/packages/arduino/hardware/zephyr/0.53.1/variants/arduino_uno_q_stm32u585xx/flash_sketch.cfg"

# Exports the path to zephyr-sketch-tool so that it can be called as 'zephyr-sketch-tool' without the full exe path
echo "export PATH=$PATH:/home/arduino/.arduino15/packages/arduino/tools/zephyr-sketch-tool/0.2.0"
} >> home/arduino/.bashrc

# Reloads sources to apply the new environment
source /home/arduino/.bashrc