# GOAL HTTP Webserver

## Properties

 + Supported methods:
  - GET
  - PUT

 + Quickstart:
  1. Call init http during GOAL application init
  1. Register resources (e.g. url, html,...) and a user callback function that should be used on resource request from a client during GOAL application setup
  1. Optional: when using simple code templates, register template usage and a user callback function once during GOAL application setup
  1. Optional: when using code template lists, register the lists and a user callback function during GOAL application setup

 + Authentication:
 Basic authentication is available. It is activated by setting it as property during resource registration. User and password are defined by the associated CM variable.

+ User callback functions:
  1. One (or more) callback funtion(s) for data transfer: download via GET oder upload via POST. The user may uilitze macros for the interaction with the webserver.
  1. Optional: One (or more) callback function(s) for template replacement when this feature is active.

##Limitations

 + PUT method handling only supports the type "multipart/form-data". 
 + When uploading files with a PUT request, only 1 file per request is supported.
   
