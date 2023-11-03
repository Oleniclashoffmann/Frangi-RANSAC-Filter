#include "save_file.h"

void save_file::save(std::string path, cv::Point2f tip, std::chrono::duration<double> time, double angle)
{
    std::ofstream outfile;

    outfile.open("C:/Users/ohoff/Documents/Bachelorarbeit/Schreiben/Erro analysis/Angle/Frangi/Fulneedle-judith/Angle/20grad.txt", std::ios_base::app); // append instead of overwrite

    if (!outfile) {
        std::cerr << "Could not open the file!" << std::endl;
     
    }
    //outfile << path << ", " << tip.x << ", " << tip.y << ", " << time.count() <<std::endl;
    outfile << angle << std::endl; 
    outfile.close();

}
