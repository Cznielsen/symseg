/*****************************************************************************/
/*  Copyright (c) 2017, Aleksandrs Ecins                                     */
/*  All rights reserved.                                                     */
/*                                                                           */
/*  Redistribution and use in source and binary forms, with or without       */
/*  modification, are permitted provided that the following conditions       */
/*  are met:                                                                 */
/*                                                                           */
/*  1. Redistributions of source code must retain the above copyright        */
/*  notice, this list of conditions and the following disclaimer.            */
/*                                                                           */
/*  2. Redistributions in binary form must reproduce the above copyright     */
/*  notice, this list of conditions and the following disclaimer in the      */
/*  documentation and/or other materials provided with the distribution.     */
/*                                                                           */
/*  3. Neither the name of the copyright holder nor the names of its         */
/*  contributors may be used to endorse or promote products derived from     */
/*  this software without specific prior written permission.                 */
/*                                                                           */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS      */
/*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT        */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     */
/*  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   */
/*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    */
/*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     */
/*****************************************************************************/

// PCL
#include <pcl/io/ply_io.h>
#include <pcl/common/time.h>

// Utilities includes
#include "filesystem/filesystem.hpp"
#include "eigen.hpp"

// Occupancy map
#include "occupancy_map.hpp"

// Symseg
#include "scene_oversegmentation.hpp"
#include "reflectional_symmetry_detection_scene.hpp"

// Project includes
#include "vis.hpp"

typedef pcl::PointXYZRGBNormal PointNC;

////////////////////////////////////////////////////////////////////////////////
void parseCommandLine(int argc, char** argv, std::string &inputCloudPath, std::string &outputDirnamePath, bool &visualize, bool &save)
{
  inputCloudPath = "";
  outputDirnamePath = "";
  visualize = true;
  save = false;
  
  // Check parameters
  for (size_t i = 1; i < static_cast<size_t>(argc); i++)
  {
    std::string curParameter (argv[i]);
        
    if (curParameter == "-novis")
      visualize = false;   
    
    else if (curParameter == "-save")
      save = true;
        
    else if (inputCloudPath == "" && curParameter[0] != '-')
      inputCloudPath = curParameter;

    else if (outputDirnamePath == "" && curParameter[0] != '-')
      outputDirnamePath = curParameter;
    
    else 
      std::cout << "Unknown parameter '" << curParameter << "'" << std::endl;
  }  
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{  
  //----------------------------------------------------------------------------
  // Parse command line
  //----------------------------------------------------------------------------
  
  std::string inputCloudPath, outputDirnamePath;
  bool visualize, save;
  parseCommandLine(argc, argv, inputCloudPath, outputDirnamePath, visualize, save);

  //----------------------------------------------------------------------------
  // Generate paths and check command line arguments
  //----------------------------------------------------------------------------
    
  if (!utl::isFile(inputCloudPath))
  {
    std::cout << "Couldn't find pointcloud file '" << inputCloudPath << "'" << std::endl;
    return -1;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  ///////////////////////           PARAMETERS           ///////////////////////
  //////////////////////////////////////////////////////////////////////////////
  
  // Scene downsample parameters
  float voxel_size = 0.005f;

//   // Reflectional symmetry detection parameters
//   sym::ReflSymDetectParams reflDetParams;
//   reflDetParams.voxel_size                  = 0.0f;                           // Voxel size used in vixelgrid downsample.
//   reflDetParams.num_angle_divisions         = 5;                              // Controls number of initial reflectional symmetry candidate proposals.
//                                                                               // Higher value - higher chance of reconvering correct symmetry at the cost of more false
//                                                                               // positives and longer runtime.
//   reflDetParams.flatness_threshold          = 0.005f;
//   reflDetParams.refine_iterations           = 20;                             // Max number of optimization iterations used to refine the candidate symmetries.
//   
//   reflDetParams.max_correspondence_reflected_distance = 0.01f;                // Maximum allowed distance between the reflections of two points forming a symmetric 
//                                                                               // correspondence.
//   reflDetParams.max_occlusion_distance                = 0.03f;                // Occlusion distances are clipped to this value.
//   reflDetParams.min_inlier_normal_angle               = pcl::deg2rad(15.0f);
//   reflDetParams.max_inlier_normal_angle               = pcl::deg2rad(20.0f);
//     
//   reflDetParams.max_occlusion_score           = 0.01f;                        // Maximum allowed occlusion score for a valid symmetry.
//   reflDetParams.min_cloud_inlier_score        = 0.5f;                         // Minimum allowed symmetric correspondence quality score normalized by the total number
//                                                                               // of points in the cloud. Scale [0, 1].
//   reflDetParams.min_corresp_inlier_score      = 0.8f;                         // Minimum allowed symmetric correspondence quality score normalized by the total number
//                                                                               // of symmetric correspondences. Scale [0, 1].
//   
//   reflDetParams.symmetry_min_angle_diff       = pcl::deg2rad(7.0);
//   reflDetParams.symmetry_min_distance_diff    = 0.01f;
//   reflDetParams.max_reference_point_distance  = 0.3f;

//   /home/aecins/Desktop/cloud_new_text.ply
//     // Reflectional symmetry detection parameters
//   sym::ReflSymDetectParams reflDetParams;
//   reflDetParams.voxel_size                  = 0.1f;                           // Voxel size used in vixelgrid downsample.
//   reflDetParams.num_angle_divisions         = 5;                              // Controls number of initial reflectional symmetry candidate proposals.
//                                                                               // Higher value - higher chance of reconvering correct symmetry at the cost of more false
//                                                                               // positives and longer runtime.
//   reflDetParams.flatness_threshold          = 0.005f;
//   reflDetParams.refine_iterations           = 20;                             // Max number of optimization iterations used to refine the candidate symmetries.
//   
//   reflDetParams.max_correspondence_reflected_distance = 0.01f;                // Maximum allowed distance between the reflections of two points forming a symmetric 
//                                                                               // correspondence.
//   reflDetParams.max_occlusion_distance                = 0.03f;                // Occlusion distances are clipped to this value.
//   reflDetParams.min_inlier_normal_angle               = pcl::deg2rad(40.0f);  // A symmetric correspondence with a correspondence angle lower than this has a
//                                                                               // the highest symmetric correspondence score i.e. 0.
//   reflDetParams.max_inlier_normal_angle               = pcl::deg2rad(60.0f);  // A symmetric correspondence with a correspondence angle greater than this has a
//                                                                               // the lowest symmetric correspondence score i.e. 1.
//     
//   reflDetParams.max_occlusion_score           = 0.01f;                        // Maximum allowed occlusion score for a valid symmetry.
//   reflDetParams.min_cloud_inlier_score        = 0.5f;                         // Minimum allowed symmetric correspondence score normalized by the total number
//                                                                               // of points in the cloud. Scale [0, 1].
//   reflDetParams.min_corresp_inlier_score      = 0.8f;                         // Minimum allowed symmetric correspondence score normalized by the total number
//                                                                               // of symmetric correspondences. Scale [0, 1].
//   
//   reflDetParams.symmetry_min_angle_diff       = pcl::deg2rad(7.0);
//   reflDetParams.symmetry_min_distance_diff    = 0.01f;
//   reflDetParams.max_reference_point_distance  = 0.3f;

  
    // Reflectional symmetry detection parameters
  sym::ReflSymDetectParams reflDetParams;
  reflDetParams.voxel_size                  = 0.01f;
  reflDetParams.num_angle_divisions         = 5;
  reflDetParams.flatness_threshold          = 0.005f;
  reflDetParams.refine_iterations           = 20;
  
  reflDetParams.max_correspondence_reflected_distance = 0.01f;
  reflDetParams.max_occlusion_distance                = 0.03f;
  reflDetParams.min_inlier_normal_angle               = pcl::deg2rad(15.0f);
  reflDetParams.max_inlier_normal_angle               = pcl::deg2rad(20.0f);
    
  reflDetParams.max_occlusion_score           = 0.01f;
  reflDetParams.min_cloud_inlier_score        = 0.5f;
  reflDetParams.min_corresp_inlier_score      = 0.8f;
  
  reflDetParams.symmetry_min_angle_diff       = pcl::deg2rad(7.0);
  reflDetParams.symmetry_min_distance_diff    = 0.01f;
  reflDetParams.max_reference_point_distance  = 0.3f;  
  
  // Occupancy map parameters
  float occupancyMapBBXInflationRadius = 0.15f;                                    // Inflation radius of the distance map bounding box relative to the scene cloud bounding box  
  float occupancyMapMaxDistance = reflDetParams.max_occlusion_distance;
  
  //////////////////////////////////////////////////////////////////////////////
  ///////////////////////           DATA LOAD            ///////////////////////
  //////////////////////////////////////////////////////////////////////////////

  std::cout << "Loading data..." << std::endl;
  
  pcl::PointCloud<PointNC>::Ptr sceneCloudHighRes  (new pcl::PointCloud<PointNC>);
  if (pcl::io::loadPLYFile (inputCloudPath, *sceneCloudHighRes))
    return -1;

  //----------------------------------------------------------------------------
  // Downsample the scene.
  //----------------------------------------------------------------------------
  
  std::cout << "Downsampling input pointcloud..." << std::endl;
  double start = pcl::getTime ();
  double totalStart = pcl::getTime ();
  
  pcl::PointCloud<PointNC>::Ptr sceneCloud (new pcl::PointCloud<PointNC>);
  
  utl::Downsample<PointNC> ds;
  ds.setInputCloud(sceneCloudHighRes);
  ds.setDownsampleMethod(utl::Downsample<PointNC>::AVERAGE);
  ds.setLeafSize(voxel_size);
  ds.filter(*sceneCloud);
  
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////           REFLECTIONAL           //////////////////////
  //////////////////////////////////////////////////////////////////////////////
    
  //----------------------------------------------------------------------------
  // Symmetry detection
  //----------------------------------------------------------------------------
    
  std::cout << "Detecting reflectional symmetry..." << std::endl;
  start = pcl::getTime (); 
  
  std::vector<sym::ReflectionalSymmetry>  reflSymmetry;
  std::vector<std::vector<int> >          reflSymmetrySupport;
  utl::Map dummy_segments (1);
  for (size_t point_id = 0; point_id < sceneCloud->size(); point_id++) {
    dummy_segments[0].push_back(point_id);
  }
  
  if (  !detectReflectionalSymmetryScene<PointNC> ( sceneCloud,
                                                    nullptr,
                                                    dummy_segments,
                                                    reflDetParams,
                                                    reflSymmetry,
                                                    reflSymmetrySupport ))
  {
    std::cout << "Could not detect reflectional symmetries." << std::endl;
    return -1;
  }

  std::cout << "  " << reflSymmetry.size() << " symmetries detected." << std::endl;
  std::cout << "  " << (pcl::getTime() - start) << " seconds." << std::endl;  

  //////////////////////////////////////////////////////////////////////////////
  std::cout << "----------------------------" << std::endl;
  float execution_time = (pcl::getTime() - totalStart);
  std::cout << "Total time: " << execution_time << " seconds" << std::endl;

  // // Save timing information to a file.
  // std::ofstream outfile;
  // outfile.open("./reflectional_segmentation_timings.txt", std::ios_base::app);
  // outfile << utl::getBasename(sceneDirname) << ": " << execution_time << "\n";
  
  //////////////////////////////////////////////////////////////////////////////
  /////////////////////           VISUALIZATION           //////////////////////
  //////////////////////////////////////////////////////////////////////////////

  if (!visualize)
    return 0;
    
  //Print instructions
  std::cout << "-------------------------------" << std::endl;
  std::cout << "|   Visualization controls    |" << std::endl;
  std::cout << "-------------------------------" << std::endl;
  std::cout << "| NUMPAD keys 1-2   | switch between different processing steps" << std::endl;
  std::cout << "| Arrow keys        | switch between different segments/symmetries" << std::endl;
  std::cout << "| NUMPAD Delete     | visualize occlusion space" << std::endl;
  std::cout << "-------------------------------" << std::endl;

  VisState visState;
  pcl::visualization::PCLVisualizer visualizer;
  Eigen::Vector4f cloudCentroid;
  pcl::compute3DCentroid<PointNC>(*sceneCloud, cloudCentroid);
  
  visualizer.setCameraPosition (  0.0, 0.0, -1.0,   // camera position
                                  cloudCentroid[0], cloudCentroid[1], cloudCentroid[2],   // viewpoint
                                  0.0, -1.0, 0.0,   // normal
                                  0.0);            // viewport
  visualizer.setBackgroundColor (utl::bgColor.r, utl::bgColor.g, utl::bgColor.b);
  visualizer.registerKeyboardCallback(keyboard_callback, (void*)(&visState));  
  visState.updateDisplay_ = true;
    
  while (!visualizer.wasStopped())
  {
    // Update display if needed
    if (visState.updateDisplay_)
    {
      // First remove everything
      visualizer.removeAllPointClouds();
      visualizer.removeAllShapes();
      visualizer.removeAllCoordinateSystems();
      visState.updateDisplay_ = false;
      
      // Then add things as required
      if (visState.cloudDisplay_ == VisState::CLOUD)
      {
        utl::showPointCloudColor<PointNC>(visualizer, sceneCloud, "cloud", visState.pointSize_);
        if (visState.showNormals_)
          utl::showNormalCloud<PointNC>(visualizer, sceneCloud, 10, 0.02, "normals", visState.pointSize_, utl::green);
        
        visualizer.addText("Original cloud", 0, 150, 24, 1.0, 1.0, 1.0);
      }
            
      else if ( visState.cloudDisplay_ == VisState::REFLECTIONAL_SYMMETRIES)
      {
        std::vector<std::vector<int> > symmetrySegmentsDisplay;
        std::vector<sym::ReflectionalSymmetry> symmetryDisplay;
        std::vector<int> symmetryDisplayIds;
        std::string text;

        visState.segIterator_ = utl::clampValueCircular<int>(visState.segIterator_, 0, reflSymmetry.size()-1);
        int symId = visState.segIterator_;
        
        utl::showFGSegmentationColor<PointNC>(visualizer, sceneCloud, reflSymmetrySupport[symId], "object", visState.pointSize_);
        
        // Show symmetry
        if (visState.showSymmetry_)
          sym::showReflectionalSymmetry(visualizer, reflSymmetry[symId], "symmetry", 0.2);
        
        visualizer.addText("Reflectional symmetry ", 0, 150, 24, 1.0, 1.0, 1.0);
        visualizer.addText("Symmetry " + std::to_string(visState.segIterator_+1) + " / " + std::to_string(reflSymmetry.size()), 15, 125, 24, 1.0, 1.0, 1.0);        
      }
    }
    
    // Spin once
    visualizer.spinOnce();
    boost::this_thread::sleep (boost::posix_time::milliseconds (10));
  }
  
  return 0;
}