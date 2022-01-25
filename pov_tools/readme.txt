
# create_POV_data_files (script)
Use ProffieOS POV Tools to create Persistence-Of-Vision images that show when you swing the blade.  
This script utilizes the existing Makefile in the pov_tools folder.  
Images can be created without this script via command line by typing 'make'.  
Arguments may be provided as 'OPTIONS=--height=N  --length=N --offset=N'  

Under the hood:   
pnmtorle - Converts a full-color image to hex data to be included in the code.  
pgmtorle - Converts a single-color image to hex data to be included in the code.  
pnmwindshieldwiper - Scales the image and bends it so that it will work on a swinging blade.  
pnmquantizedtorle - Quantizes images to 8bit 256 colors to balance using color images and not taking up too much memory.  

### Operating System Requirements:  
### Windows (Windows 10 at time of writing):  

Install Windows Subsystem for Linux  
- Get Ubuntu App in the Microsoft Store. (required for “bash” and other commands). Download but don't choose open once it finishes installing.  
https://www.microsoft.com/en-us/p/ubuntu/9nblggh4msv6?activetab=pivot:overviewtab  
- Type “Turn Windows features on or off” into the Windows search, open the top result, scroll down and enable "Windows Subsystem for Linux".  
- Click "OK", then click "Reboot Now".  
- Open the Ubuntu app and wait while it says "Installing, this may take a few minutes...".  
- Choose a username and password. Once you get a green prompt (should be username+computername), close the window.  
- Type “WSL” into the windows search and click that cheeky penguin image.  

Install g++ and netpbm  
- It goes without saying that you hit the return key after each of the following entries.  
- In the command line window that opened, type "sudo apt-get update". Enter your password.  
- When that finishes, type "sudo apt-get install build-essential gdb".  
- Answer the question with "y".  
- When that finishes, type "sudo apt install netpbm".  
- Answer the question with "y".  

### MacOS:
- Install Homebrew https://treehouse.github.io/installation-guides/mac/homebrew  
- Install netpbm (In Terminal, type "brew install netpbm").  

### Linux
- Install netpbm http://netpbm.sourceforge.net/  

---------------------------------------------------------------------------------------

### Make POV files

- Put one or more .png images that you want to convert into the pov_tools folder.  
If you have no images, you can run the script anyway and it can download the sample Star Wars logo and process it for you :)  
File names that contain 'preview' will be ignored as valid source image files. 

To run the script:  
- Windows: Double click "create_POV_data_files-Win.bat"  
    (If Microsoft Defender pops up a warning, click "More Info" then "Run anyway")  
    Mac or Linux: Double click "create_POV_data_files"  

- Choose a sizing option from the list. There are some preset sizes plus 'Height entry only' and 'Custom size' options.  
(Height is the number of pixels in the blade)  
You may want to choose 'Custom' first to get an idea of what gets calculated, then the presets will make more sense.  
Once familiar, you can customize the presets by editing that section of the script.  

All valid .png files in the pov_tools folder will be processed to the setting you chose.
- Check the resulting *preview.png in the pov_tools folder. It should look warped like a windshield shape.  
- The image data files are written directly to the ProffieOS/styles folder.  
They will be formatted "sourceFileName_height_colorProfile_POV_data.h"  
If you used a Single Color source file, use the resulting SC_POV_data.h file.  
If you used a Full Color source file, then use the resulting FC_POV_data.h file.  
If you used a Full Color source file, memory use might be less by choosing the 8b_POV_data.h file.  
- A running log of your conversion history with the setting used is written to POV_conversions.log.  

- To use the POV data file for a blade style, first tell the OS which one with the following line added to your config file  
in the CONFIG_TOP section: `#define POV_INCLUDE_FILE "dataFileNameHere.h"`  

- Then you just use the POV blade style in a preset:  
```
{ "Font", "tracks/track.wav",
   &style_pov, 
  "my_pov"},
  ```

Swing the blade in a steady, quick movement to see the image in the air. A long-exposure camera capture showcases the result best.  
Enjoy!  (⌐■_■) NoSloppy
-- credits: Thanks profezzorn for overseeing/providing options, kaQnub for Windows testing/contributions.
