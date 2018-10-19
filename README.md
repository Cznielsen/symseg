# SymSeg: Model-Free Object Segmentation in Point Clouds Using the Symmetry Constraint.

SymSeg is a model-free method for segmenting objects and recovering their symmetries from multiple-view 3D pointclouds. The algorithm takes a pointcloud and an occupancy map of the scene as input and returns a set of object segmentation masks and corresponding symmetries. Please cite the following work if you use it for your research:

*Ecins, Aleksandrs; Ferm&uuml;ller, Cornelia; Aloimonos, Yiannis (2018) [Seeing Behing the Scene: Using Symmetry To Reason About Objects in Cluterred Environments](http://users.umiacs.umd.edu/~aecins/projects/symseg/symmetry_segmentation_IROS18.pdf). International Conference on Intelligent Robots.* [[Project Page](http://users.umiacs.umd.edu/~aecins/projects/symseg/)]

This repository contains the C++ implementation of the algorithm and a sample test scene. Additional test scenes can be downloaded from the [Cluttered Tabletop Dataset](http://www.umiacs.umd.edu/~aecins/projects/symseg/dataset/CTD.tar.gz) (160 Mb).

## Dependencies ##
- [PCL](https://github.com/PointCloudLibrary/pcl)
- [OctoMap](https://github.com/OctoMap/octomap) (with dynamicEDT3D)
- [protobuffer](https://developers.google.com/protocol-buffers/)

Tested in Ubuntu 14.04 and 16.04 with PCL 1.8.0 and Octomap 1.8.1


## Building ##

To checkout and build *symseg* in the `build` directory, execute the following in a terminal:

```
git clone https://github.com/aecins/symseg.git
cd symseg
mkdir build
cd build
cmake ..
make -j
```

## Segmentation Examples ##
The `examples` directory provides examples for three different segmentation modes:
- `rotational_segmentation` segments rotational objects
- `reflectional segmentation` segments reflectional objects
- `full_segmentation` segments rotational objects first, then segment reflectional objects

To segment the provided sample scene execute the following from the `bin` directory:
```
./rotational_segmentation ../sample_scene
```

## Symmetry Detection Examples ##

Additionally we provide code for detecting symmetries in pointclouds of single objects. Note that input pointclouds must contain a single object and must have consistently oriented normals. To detect symmetries execute from the `bin` directory:
```
./reflectional_detection ../sample_object_cloud.ply
```