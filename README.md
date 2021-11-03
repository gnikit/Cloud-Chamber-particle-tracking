# Cloud-Chamber-particle-tracking

Tracks the trajectories of Alpha and Beta particles in a Cloud Chamber detector and exports the tracked trajectories along with the linear fit of the trajectories. The program utilises the **EasyBMP** library to operate on a series of images that are snapshots of the Cloud Chamber detector at various time.

## Example

Input image with alpha and beta particle trajectories

![alt](images/scene00044.bmp)

![alt](images/output.bmp)

## Setup, Compile & Run

Configure the project

```bash
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Compile and install

```bash
make -j
make install
```

Run the analysis for the cloud chamber data

```bash
./bin/cloud-chamber
```
