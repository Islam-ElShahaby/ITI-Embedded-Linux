1. **Create the Script**
   Write a script in your host machine using U-Boot commands (e.g., `uboot-script.txt`).

2. **Compile the Script**
   Compile the text file into a U-Boot image using `mkimage`:
   ```bash
   mkimage -T script -C none -A arm -d uboot-script.txt booting
   ```

3. **Deploy to TFTP**
   Copy the `booting` script image to your TFTP server directory:
   ```bash
   sudo cp booting /srv/tftp/
   ```

4. **Run on Target**
   Load the script to the DRAM and source the address where the script is loaded:
   ```bash
   tftp $kernel_addr_r booting
   source $kernel_addr_r
   ```