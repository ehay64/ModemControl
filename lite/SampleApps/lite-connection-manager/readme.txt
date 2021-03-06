This documents provide information about the Connection manager application.
This application supports:

    1. Starting an UMTS Data Session.

    2. Starting a LTE Data Session.

    3. Starting a CDMA Data Session.

    4. Starting a RUIM Data Session

    5. Stopping the currently active Data Session started .

    6. Displaying all the profiles stored on the device.

    7. Displaying the settings for a particular profile stored on the device.

    8. Creating a Profile on the device.

    9. Modifying the settings of an existing profile stored on the device.

    10. Deleting a profile stored on the device.

************
Compilation
************
- By default, the application is compiled for i86 platform.
  "make"

- The application can be compiled for different platform using the CPU option:
  "make CPU=<platform>"
  where <platform> = arm9 in case 0f ARM and
                     ppc  in case of Power PC.
- For arm9 and powerpc, platform specific tool chains are required. These must be
  installed to the $(HOME) directory once, and can be used for all SDK workspaces.
  If they do not exist, 
  "make CPU=arm9" and
  "make CPU=ppc"  will simply fail.

**********
Execution
**********
The following operations can be performed using the application:

    a)  Starting an UMTS Data Session.
        - The application will display all the profiles stored on the device.
        - User will be prompted to select the profile index from the displayed list
          using which the UMTS data session will be created.

    b)  Starting an LTE Data Session.
        - The application will display the three possible IP Family preferences.
        - User will be prompted to select an IP Family preference which will
          subsequently be used to start a data session.
        - The application will display all the profiles stored on the device.
        - User will be prompted to select the profile index from the displayed list
          using which the LTE data session will be created.

    c)  Stopping the currently active Data Session started using option a or b above.

    d)  Displaying all the profiles stored on the device.

    e)  Displaying the settings for a particular profile stored on the device.
        - The application will prompt the user to provide the profile index for which detailed
          information needs to be displayed.

    f) Creating a Profile on the device.
        - The application will display all the profiles stored on the device.
        - User will be prompted to input all the values corresponding to the new profile.
        - The application will display the updated list of all the profiles on the device including
          the new created profile.

    g) Modifying the settings of an existing profile stored on the device.
        - The application will display all the profiles stored on the device.
        - User will be prompted to select the profile id from the list displayed above whose value
          needs to be modified.
        - User will then be prompted to input all the values corresponding to the profile that needs
          to be modified.
        - The application will display the updated list of all the profiles on the device including
          the profile whose value is updated.

    h)  Deleting a profile stored on the device.
        - The application will display all the profiles stored on the device.
        - User will be prompted to select the profile index from the displayed list for the profile
          to be deleted.
        - The application will display the updated list of all the profiles existing on the device.

Also the values will be updated with different parameters of devices to /var/log/user.log file.
The values will be updated in log file depending on the action performed by application and device.

The Connection Manager Display (CMD) provides a real-time device, network, and data session information. The CMD can
be viewed by executing the following in a terminal second terminal:

    tail -f /var/log/user.log | grep connectionmgr
