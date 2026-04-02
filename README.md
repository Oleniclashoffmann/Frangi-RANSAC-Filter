# Real-Time Needle Tip Detection in Ultrasound Images

> **Bachelor's Thesis Project** — 3 of 3 Real-time needle axis and tip estimation in ultrasound images using a Frangi Filter and a RANSAC algorithm for the tip estimation.

---

## Problem Statement

Accurate needle tip localization during ultrasound-guided medical procedures (e.g. biopsies, regional anesthesia) is critical for patient safety. However, needles are often poorly visible in ultrasound images due to noise, speckle artifacts, and tissue interference. Manual tracking by clinicians is error-prone and limits procedural efficiency. This project addresses the challenge by implementing a **fully automated, multi-stage image processing pipeline** that enhances needle visibility, detects the needle trajectory, and precisely estimates the tip position — all in real time.

---

## Algorithm Overview

The system combines four classical computer vision and signal processing techniques into a sequential pipeline:

1. **Frangi Vesselness Filter** — enhances elongated tubular structures (needles) using multi-scale Hessian analysis
2. **RANSAC** — robustly fits a line to the detected needle pixels, rejecting outliers from noise
3. **Bayesian Tip Estimation** — locates the needle tip along the fitted line using a custom probability model
4. **Kalman Filter** — temporally smooths tip position estimates across sequential frames

Each algorithm was selected for its specific strength: Frangi for structure enhancement, RANSAC for noise robustness, Bayesian estimation for sub-pixel precision, and Kalman for temporal consistency.

---

## Pipeline Steps

```
Ultrasound Image → Preprocessing → Frangi Filter → RANSAC → Tip Estimation → Kalman Filter → Output
```

### 1. Preprocessing
- Converts input to grayscale and applies **Gaussian blur** (7×7 kernel) for noise reduction
- Applies **adaptive thresholding** to isolate bright structures
- **Rotates** the image to align the needle insertion axis for consistent downstream processing

### 2. Frangi Vesselness Filter
Implements the [Frangi et al. (1998)](https://link.springer.com/chapter/10.1007/BFb0056195) multi-scale vesselness filter, which enhances elongated tubular structures while suppressing blob-like noise:
- Computes the **2D Hessian matrix** at multiple scales via second-order Gaussian derivative kernels
- Performs **eigenvalue decomposition** to analyze local image structure
- Calculates a **vesselness response** based on eigenvalue ratios (blobness measure $R_B$) and structure magnitude ($S$):

$$V(\sigma) = \exp\left(-\frac{R_B^2}{2\beta_1^2}\right) \cdot \left(1 - \exp\left(-\frac{S^2}{2\beta_2^2}\right)\right)$$

- Applies **multi-scale analysis** ($\sigma \in [1, 2]$) and retains the maximum response across scales

### 3. RANSAC Line Fitting
Uses **Random Sample Consensus (RANSAC)** to robustly fit a line to the enhanced needle pixels:
- Extracts non-zero pixel coordinates from the Frangi filter output as candidate data points
- Iteratively samples random point pairs and fits a **linear regression** model (up to 700 iterations)
- Classifies inliers using a **distance threshold** (3.0 px for tissue / 20.0 px for gel phantom)
- Selects the model with the **maximum inlier consensus** as the estimated needle axis
- Defines a **Region of Interest (ROI)** for subsequent frames to reduce search space

### 4. Probabilistic Tip Estimation
Implements a **custom Bayesian probability model** to locate the needle tip along the detected line:
- For each candidate point along the needle axis (±10 px corridor), computes pixel intensity distributions **before and after** the point
- Calculates conditional probabilities based on the ratio of white (needle) to black (background) pixels
- Ranks all candidate points by a combined **posterior probability score** $P(N^+ | n, x, y)$
- Selects the point with the **highest probability** as the estimated tip position

### 5. Kalman Filter
Applies a **linear Kalman filter** for temporal smoothing across sequential frames:
- **State vector**: $[x, y, v_x, v_y]^T$ — position and velocity in 2D
- **Prediction step**: propagates state using constant-velocity motion model
- **Correction step**: fuses prediction with the measured tip position
- Automatically **resets** at each new needle insertion sequence (every 25 frames)
- Outputs both predicted and corrected positions for visualization

---

## Tech Stack

| Technology | Purpose |
|---|---|
| **C++17** | Core implementation language |
| **OpenCV** | Image processing, connected components, line drawing, matrix operations |

---

## Project Structure

```
MLESAC/
├── main.cpp              # Entry point — orchestrates the full pipeline
├── Preprocessing.cpp/h   # Image preprocessing module
├── Frangi.cpp/h          # Frangi vesselness filter
├── RANSAC.cpp/h          # RANSAC line fitting
├── tip_estimation.cpp/h  # Probabilistic tip estimation
├── Kalman.cpp/h          # Kalman filter
├── save_file.cpp/h       # Result export
└── Recources/            # Reference implementations and resources
```

---

## Component Responsibilities

| Component | Responsibility |
|---|---|
| **`Preprocessing`** | Grayscale conversion, Gaussian blur (7×7), adaptive thresholding, image rotation to align needle axis |
| **`Frangi`** | Multi-scale Hessian computation, eigenvalue decomposition, vesselness response calculation across scale space ($\sigma = 1 \dots 2$) |
| **`RANSAC`** | Random sampling of point pairs, linear regression fitting, inlier/outlier classification, consensus-based model selection (700 iterations) |
| **`tip_estimation`** | Bayesian probability computation along the needle axis (±10 px corridor), pixel intensity ratio analysis, maximum-probability tip selection |
| **`Kalman`** | 4-state linear Kalman filter ($[x, y, v_x, v_y]^T$), constant-velocity prediction, measurement correction, auto-reset per insertion cycle |
| **`save_file`** | Exports per-frame results (tip coordinates, processing time, insertion angle) to text file for offline analysis |
| **`main`** | Sequentially loads frames, invokes each pipeline stage, manages ROI propagation between frames, handles visualization and timing |

---

## Prerequisites

- **OpenCV 4.x** — install via [official guide](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html) or package manager
- **C++ compiler** with C++11 support (MSVC, GCC, or Clang)
- **Visual Studio** (Windows) or adapt the build for CMake on other platforms

---

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/Frangi-RANSAC-Filter.git
   ```
2. Open `MLESAC.sln` in Visual Studio
3. Set your image dataset path in `main.cpp`:
   ```cpp
   std::string path = "path/to/your/ultrasound/dataset/";
   ```
4. Build and run the solution

### Output

The program displays:
- **Frangi filter response** — enhanced vessel/needle structures
- **RANSAC line fit** — detected needle axis overlaid on the image
- **Tip estimation** — estimated (blue), predicted (red), and Kalman-corrected (green) tip positions
- **Console logs** — per-frame timing, estimated insertion angle, and tip coordinates
- **Data export** — results saved to text file for further analysis

---

## Configuration Parameters

| Parameter | File | Default | Description |
|---|---|---|---|
| `sigma_start` / `sigma_end` / `sigma_step` | `Frangi.h` | 1 / 2 / 1 | Scale range for the Frangi multi-scale analysis |
| `BetaOne` / `BetaTwo` | `Frangi.h` | 0.3 / 5 | Sensitivity constants for blobness and background noise suppression |
| `m_k` | `RANSAC.h` | 700 | Maximum RANSAC iterations |
| `m_n` | `RANSAC.h` | 10 | Minimum data points to estimate parameters |
| `m_t` | `RANSAC.h` | 3.0 | Inlier distance threshold (3.0 for tissue, 20.0 for gel) |
| `m_d` | `RANSAC.h` | 10 | Minimum inliers required to accept a model |
| `kp` | `main.cpp` | 0.5 | Kalman process noise covariance scaling factor |
| `angle` | `main.cpp` | 100 | Image rotation angle to align needle axis |
| Insertion cycle | `main.cpp` | 25 frames | Number of frames per insertion (triggers Kalman reset) |

---

## Context

This project was developed as part of a **Bachelor's thesis** on automated needle detection in ultrasound imaging. The algorithm is designed for real-time processing of sequential ultrasound frames and outputs the estimated needle tip position, axis angle, and per-frame computation time.

### References

- Frangi, A.F. et al. — *"Multiscale vessel enhancement filtering"* (MICCAI 1998)
- Fischler, M.A. & Bolles, R.C. — *"Random Sample Consensus: A Paradigm for Model Fitting"* (1981)
- Welch, G. & Bishop, G. — *"An Introduction to the Kalman Filter"* (2006)

---

## License

This project was developed as part of a Bachelor's thesis. Feel free to use it as a reference or starting point for your own work.
