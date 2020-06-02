# USB to Serial Interface Design (Computer Systems Engineering Mini Project)

### About:

The objective behind this project is to build a serial port interfacing device using an Arduino which can be connected to any type of PC via USB. This interface will be shipped with a custom C library with standard commands to read and write into the serial port and a GUI program which can be used to configure the serial port.

### Documents

- [Report](https://github.com/chandimab/usb-to-serial/raw/master/documentation/Project_Report.pdf)

### Work Break Down

The overall workload was broken down into the following major tasks.

(1) Design of the overall software and hardware architecture (abstract design)
(2) Hardware design and fabrication
(3) Feasibility study to determine approaches to ensure efficient PC-Arduino communication
(4) GUI application development
(5) High-level software protocol design

- Implementing the protocol in such a way that it is handled by the library (and not by the programmer).
- Ensuring that a programmer can easily invoke library functions to perform tasks.
- Determining the use of fixed-sized or variable-sized messages.
- Determining the types of messages to be implemented (E.g. token messages, data messages, configuration messages).
- Testing the functionality of the protocol.

(6) Design and development of a C/C++ library for the PC-side

- Developing PC-Arduino communication using serial communication classes and libraries offered by Qt toolkit libraries.
- Developing basic library functions to enable,
- Configuration of serial port parameters:
- Writing/reading of data from/to the serial port

(7) Design and development of an Arduino software

- Developing Arduino-side USART communication library functions from the scratch as standard libraries were found to be inefficient.
- Implementing and testing the designed protocol in Arduino.

(8) Writing sample applications

### Technologies:

- Qt Creator
- C++
- Qt Libraries
- Arduino
- Autodesk Eagle.
