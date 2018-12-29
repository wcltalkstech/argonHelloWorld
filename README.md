# argonHelloWorld

brew install openocd

download debugger config file: https://docs.particle.io/assets/files/nrf52-var.cfg

copy file from download folder to: /usr/local/share/openocd/scripts/target

cd /usr/local/share/openocd/scripts

openocd -f interface/cmsis-dap.cfg -f target/nrf52-var.cfg -c "telnet_port 4444"

openocd -f interface/cmsis-dap.cfg -f target/nrf52-var.cfg -c "gdb_port 3333"

in gdb
    interrupt // stops execution
    continue // start again from where you left off

