# Connected_House

OpenSource Project to allow anyone to use probes to monitor things.
- Documentation on how to create (or where to buy) the probes will are given in the wiki.
- A web interface will be provided for you to use to have a nice experience with viewing your data (Django-server). 
- The code for this interface (and server) is also given in case you want to have your own server running.

The probes will run thanks to Moteinos. But as the code is usable for Arduino, you can also use them if a radio module is added.


## What is in this github ?

### The bridge folder 

It gathers the code for the bridge moteino to use (in adruino)

### The Django_connected/connected_house

It is the folder that has the Django server in it.
Use the folder inside to run it localy (with python runserver, see the wiki for that) or deploy it.

- Inside that folder is a folder called server, this is actually all the views, html template and static file for the server.
- venv is the local environement used by the host server (like heroku) to know exactly what to use rather than the default setting. This folder might be optional to use.
- website is gather the model of the server and its migration
- db.sqlite3 is the database
- manage.py is the file needed to lunch the app (like a manifest)
- The other files are only for deployment (required for Heroku by example)

### Pictures

This folder is only use for displaying some images on the wiki.

### Probes folder

It gathers the code for the probes moteino (in aduino)

### WirelessProgramming_gateway_without_python folder

It has the code required to do wireless programming using a POST to retrieve the new code rather that a file on a computer and luching it throught a python command (and USB)

### WirelessProgramming folder

It gathers the code for the basic wireless programming. This code is available on the github of the constructor of the moteinos

### lib folder

It has all the zip libraries used by the moteinos. You will need them if you put the code in the moteinos yourself

### thinkspeak folder

It is the original server that has been given to us. It uses angular, ruby on rails and thinkspeak.

### .gitignore

Only to avoid indexing some files in github


## Team Members:

**Augustin De Vita**

**Thomas Garesse**

**Marc Lamazouère**
