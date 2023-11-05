# MLESAC
# Frangi Filter Algorithm

## Introduction
This repository contains the implementation of the Frangi filter algorithm, which is utilized for enhancing and detecting vessel-like structures in medical images. Originally named MLESAC, the project was later adapted to incorporate the Frangi filter approach. Please note that despite the historical naming, the current algorithm does not implement the MLESAC algorithm.

## Structure
The project is structured as follows:
- `main.cpp`: The entry point of the program where the image processing pipeline is initiated.
- `Preprocessing`: A class responsible for the initial preprocessing of images.
- `Frangi`: The class implementing the core Frangi filter algorithm.
- `RANSAC`: A class that was part of the original MLESAC approach, now repurposed for our use-case.
- `Kalman`: This class includes a Kalman filter, which is used for estimation enhencement.
- `tip_estimation`: Contains methods for estimating the tips of the detected vessels.
- `save_file`: Handles the saving of results.

## Prerequisites
Before running the algorithm, ensure you have the following prerequisites installed:
- install OpenCv to run the code

## Configuration
To run the algorithm, you must first set the path to your image repository in `main.cpp`. This allows the program to load and process your images.
