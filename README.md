# RPi-WebChecker
Producing a Raspberry Pi-based website status checker in Python.

## Introduction

The aim of this project was to produce a reliable website status checker with Amazon Web Services (AWS) databasing and web hosting functionality on a Raspberry Pi (RPi) Internet of Things (IoT) device. The device used a red and yellow LED to display which of two predefined endpoints (websites) are active, staying lit if there were no issues and beginning to flash if an active ping response was not received. Once the response was processed by the program and the status of the site is determined to be unreachable, an entry was securely submitted to an AWS database with the unreachable address, time of the error, and the error code; this allows for further future investigation into the causes and effects of a potential crash all the while being as easy to use, reliable, and efficient as possible.

## Video Link

https://www.youtube.com/watch?v=Hm7kzkoZ0kg
