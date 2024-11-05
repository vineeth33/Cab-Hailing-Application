# Cab hailing software

## Table of Contents
1. [Introduction](#introduction)
2. [Project Description](#project-description)
3. [Files](#files)
4. [Dependencies](#dependencies)
5. [Usage](#usage)

## Introduction

The Cab hailing software is a software application designed to facilitate the booking and allocation of cabs for users. It provides a user-friendly interface to book cabs, view available cab types, calculate estimated fares, and allocate the nearest cab based on user preferences and location information.

## Project Description

The Cab hailing software aims to simplify the process of cab booking and allocation by utilizing graph algorithms for route optimization and efficient cab allocation. It allows users to choose their pickup and drop-off locations, select from different cab types, and receive the estimated fare for their trip. The system also provides real-time information on cab availability and assigns the nearest available cab to the user's location.

## Files

The project files are organized as follows:

- `cab_types.txt`: This file contains information about the different types of cabs available, including their names, fare rates, availability status, and fare per kilometer.
- `locations.txt`: This file stores the information about different locations available for pickup and drop-off, along with their corresponding IDs,latitude and longtitude.
- `users.txt`: This file maintains user profiles and their details, including names, phone numbers, email addresses, and login credentials.
- `drivers.txt`: This file stores the details of available drivers, including their names, phone numbers, car plate numbers,cab type,latitude and longtitude.
- `cab_faqs.txt`: This file stores the frequently asked questions.
- `Minidrivers.txt`: This file stores the details of available mini drivers, including their names, phone numbers, car plate numbers,latitude and longtitude.
- `Sedandrivers.txt`: This file stores the details of available sedan drivers, including their names, phone numbers, car plate numbers,latitude and longtitude.
- `SUVdrivers.txt`: This file stores the details of available SUV drivers, including their names, phone numbers, car plate numbers,latitude and longtitude.
- `Bikedrivers.txt`: This file stores the details of available bike drivers, including their names, phone numbers, car plate numbers,latitude and longtitude.
- `Autodrivers.txt`: This file stores the details of available auto drivers, including their names, phone numbers, car plate numbers,latitude and longtitude.

## Dependencies

The project is implemented in C and requires a gcc C compiler to build and run. No additional external libraries are used. This project is optimised to run on Linux and MacOS Environment.

## Usage

1. Ensure you have a gcc C compiler installed on your system.
2. Clone this repository to your local machine.
3. Navigate to the project directory, ensure that current directory has main.c
4. Compile the source code using the following command:

```bash
gcc main.c && ./a.out
```

5. Follow the on-screen instructions to provide candidate preferences and view the allocated seats.
