#include "imgproc.h"
#include <cmath>

void imgProc::toGray(int method) {
    for(int i = 0;i<source.width();i++)
        for(int j = 0;j<source.height();j++){
            QColor col = source.pixelColor(i,j);
            int R = col.red();
            int G = col.green();
            int B = col.blue();
            int g;
            switch(method) {
            case 0:
                g = (R+G+B)/3;
                break;
            case 1:
                g = (std::max(R,std::max(G,B)) + std::min(R,std::min(G,B)))/2;
                break;
            case 2:
                g = (int)round(R*0.21+G*0.72+B*0.07);
                break;
            case 3:
                g = (int)round(R*0.3+G*0.59+B*0.11);
                break;
            case 4:
                g = (int)round(R*0.2126+G*0.7152+B*0.0722);
                break;
            case 5:
                g = std::max(R,std::max(G,B));
                break;
            case 6:
                g = std::min(R,std::min(G,B));
                break;
            case 7:
                g = (int)round(R*0.2952+G*0.5547+B*0.148);
                break;
            }
            result.setPixelColor(i,j,QColor(g,g,g));
        }
}


void imgProc::ThBinarize(int threshold, bool isAllChannels){
    if(isAllChannels) {
        for(int i = 0;i<result.width();i++)
            for(int j = 0;j<result.height();j++){
                QColor col = source.pixelColor(i,j);
                int R = col.red();
                int G = col.green();
                int B = col.blue();
                if(R < threshold) R = 0; else R = 255;
                if(G < threshold) G = 0; else G = 255;
                if(B < threshold) B = 0; else B = 255;
                    result.setPixelColor(i,j,QColor(R,G,B));
            }
    } else {
        toGray(0);
        for(int i = 0;i<result.width();i++)
            for(int j = 0;j<result.height();j++){
                int intens = result.pixelColor(i,j).red();
                if(intens<threshold)
                    result.setPixelColor(i,j,QColor(0,0,0));
                else
                    result.setPixelColor(i,j,QColor(255,255,255));
            }
    }
}

void imgProc::OtsuBinarize(bool isAllChannels){
    if(isAllChannels){
        QVector<QVector<int>> hist(3,QVector<int>(256));
        for(int i = 0;i<source.width();i++)
            for(int j = 0;j<source.height();j++){
                hist[0][source.pixelColor(i,j).red()]++;
                hist[1][source.pixelColor(i,j).green()]++;
                hist[2][source.pixelColor(i,j).blue()]++;
            }
        int Rth = GetThr(hist[0]);
        int Gth = GetThr(hist[1]);
        int Bth = GetThr(hist[2]);
        for(int i = 0;i<result.width();i++)
            for(int j = 0;j<result.height();j++){
                QColor col = source.pixelColor(i,j);
                int R = col.red();
                int G = col.green();
                int B = col.blue();
                if(R < Rth) R = 0; else R = 255;
                if(G < Gth) G = 0; else G = 255;
                if(B < Bth) B = 0; else B = 255;
                    result.setPixelColor(i,j,QColor(R,G,B));
            }

    } else {
        toGray(0);
        QVector<int> hist(256);
        for(int i = 0;i<result.width();i++)
            for(int j = 0;j<result.height();j++)
                hist[result.pixelColor(i,j).red()]++;
        int threshold = GetThr(hist);
        for(int i = 0;i<result.width();i++)
            for(int j = 0;j<result.height();j++) {
                int intens = result.pixelColor(i,j).red();
                if(intens<threshold)
                    result.setPixelColor(i,j,QColor(0,0,0));
                else
                    result.setPixelColor(i,j,QColor(255,255,255));
            }
    }
}


void imgProc::MedianBlur(int size) {
  QVector<QVector<int>> range(3,QVector<int>(size*size));
  result = source;

  for(int i = size/2; i<source.width()-size/2; i++)
      for(int j = size/2; j<source.height()-size/2; j++) {

          int k = 0;
          for(int s = i-size/2;s<i+size/2+1;s++)
              for(int t = j-size/2;t<j+size/2+1;t++)
              {
                  range[0][k] = source.pixelColor(s,t).red();
                  range[1][k] = source.pixelColor(s,t).green();
                  range[2][k] = source.pixelColor(s,t).blue();
                  k++;
              }
          std::sort(range[0].begin(),range[0].end());
          std::sort(range[1].begin(),range[1].end());
          std::sort(range[2].begin(),range[2].end());

          result.setPixelColor(i,j,QColor(range[0][size*size/2],range[1][size*size/2],range[2][size*size/2]));
      }
}

void imgProc::GaussianBlur(int size) {
    result = source;
    QVector<QVector<double>> kernel(size,QVector<double>(size));
    double sum = 0;
    double sig = size/6.0;
    for(int i = 0;i<size;i++)
        for(int j = 0;j<size;j++){
            kernel[i][j] = exp(-((size/2-i)*(size/2-i) + (size/2-j)*(size/2-j))/(2*sig*sig))
                    /(2*M_PI*sig*sig);
            sum+=kernel[i][j];
        }
    for(int i = 0;i<size;i++)
        for(int j = 0;j<size;j++)
            kernel[i][j]/=sum;
    ApplyKernel(source,result,kernel);
}

void imgProc::BoxBlur(int size) {
    result = source;

    for(int i = size/2; i<source.width()-size/2; i++)
        for(int j = size/2; j<source.height()-size/2; j++) {
            QVector<int> sums(3);
            for(int s = i-size/2;s<i+size/2+1;s++)
                for(int t = j-size/2;t<j+size/2+1;t++)
                {
                    sums[0] += source.pixelColor(s,t).red();
                    sums[1] += source.pixelColor(s,t).green();
                    sums[2] += source.pixelColor(s,t).blue();
                }
            result.setPixelColor(i,j,QColor(sums[0]/(size*size),sums[1]/(size*size),sums[2]/(size*size)));
        }
}


void imgProc::SobelEdges(){
    toGray(0);
    QVector<QVector<int>> gradx(source.width(),QVector<int>(source.height()));
    QVector<QVector<int>> grady(source.width(),QVector<int>(source.height()));
    QImage buf = result;

    for(int i = 1; i<source.width()-1; i++)
        for(int j = 1; j<source.height()-1; j++) {

            gradx[i][j] = -buf.pixelColor(i-1,j-1).red()-2*buf.pixelColor(i-1,j).red()-buf.pixelColor(i-1,j+1).red()+
                    buf.pixelColor(i+1,j-1).red()+2*buf.pixelColor(i+1,j).red()+buf.pixelColor(i+1,j+1).red();
            grady[i][j] = -buf.pixelColor(i-1,j-1).red()-2*buf.pixelColor(i,j-1).red()-buf.pixelColor(i+1,j-1).red()+
                    buf.pixelColor(i-1,j+1).red()+2*buf.pixelColor(i,j+1).red()+buf.pixelColor(i+1,j+1).red();

            int g = (int)std::round(sqrt(gradx[i][j]*gradx[i][j] + grady[i][j]*grady[i][j]));
            if (g>255) g =255;
            if (g<0) g= 0;
            result.setPixelColor(i,j,QColor(g,g,g));
        }

}

void imgProc::RobertsEdges(){
    toGray(0);
    QVector<QVector<int>> gradx(source.width(),QVector<int>(source.height()));
    QVector<QVector<int>> grady(source.width(),QVector<int>(source.height()));
    QImage buf = result;

    for(int i = 0; i<source.width()-1; i++)
        for(int j = 0; j<source.height()-1; j++) {

            gradx[i][j] = buf.pixelColor(i,j).red()-buf.pixelColor(i+1,j+1).red();
            grady[i][j] = buf.pixelColor(i+1,j).red()-buf.pixelColor(i,j+1).red();

            int g = (int)std::round(sqrt(gradx[i][j]*gradx[i][j] + grady[i][j]*grady[i][j]));
            if (g>255) g =255;
            if (g<0) g= 0;
            result.setPixelColor(i,j,QColor(g,g,g));
        }
}

void imgProc::PrewittEdges(){
    toGray(0);
    QVector<QVector<int>> gradx(source.width(),QVector<int>(source.height()));
    QVector<QVector<int>> grady(source.width(),QVector<int>(source.height()));
    QImage buf = result;

    for(int i = 1; i<source.width()-1; i++)
        for(int j = 1; j<source.height()-1; j++) {

            gradx[i][j] = buf.pixelColor(i-1,j-1).red() + buf.pixelColor(i-1,j).red() + buf.pixelColor(i-1,j+1).red()-
                    buf.pixelColor(i+1,j-1).red() - buf.pixelColor(i+1,j).red() - buf.pixelColor(i+1,j+1).red();
            grady[i][j] = buf.pixelColor(i-1,j-1).red() + buf.pixelColor(i,j-1).red() + buf.pixelColor(i+1,j-1).red()-
                    buf.pixelColor(i-1,j+1).red() - buf.pixelColor(i,j+1).red()- buf.pixelColor(i+1,j+1).red();

            int g = (int)std::round(sqrt(gradx[i][j]*gradx[i][j] + grady[i][j]*grady[i][j]));
            if (g>255) g =255;
            if (g<0) g= 0;
            result.setPixelColor(i,j,QColor(g,g,g));
        }
}


void imgProc::Dilate(QImage buf, int size){

    for(int i = size; i<buf.width()-size; i++)
        for(int j = size; j<buf.height()-size; j++) {
            QVector<QVector<int>> final(3);
            for(int s = i-size;s<i+size+1;s++)
                for(int t = j-size;t<j+size+1;t++)
                {
                        final[0].push_back(buf.pixelColor(s,t).red());
                        final[1].push_back(buf.pixelColor(s,t).green());
                        final[2].push_back(buf.pixelColor(s,t).blue());
                }

            int R = *std::max_element(final[0].begin(),final[0].end());
            int G = *std::max_element(final[1].begin(),final[1].end());
            int B = *std::max_element(final[2].begin(),final[2].end());
            result.setPixelColor(i,j,QColor(R,G,B));
        }
}

void imgProc::Erode(QImage& buf, int size){
    for(int i = size; i<buf.width()-size; i++)
        for(int j = size; j<buf.height()-size; j++) {
            QVector<QVector<int>> final(3);
            for(int s = i-size;s<i+size+1;s++)
                for(int t = j-size;t<j+size+1;t++)
                {
                        final[0].push_back(buf.pixelColor(s,t).red());
                        final[1].push_back(buf.pixelColor(s,t).green());
                        final[2].push_back(buf.pixelColor(s,t).blue());
                }

            int R = *std::min_element(final[0].begin(),final[0].end());
            int G = *std::min_element(final[1].begin(),final[1].end());
            int B = *std::min_element(final[2].begin(),final[2].end());
            result.setPixelColor(i,j,QColor(R,G,B));
        }
}

void imgProc::Opening(QImage& buf, int size){
    Erode(buf,size);
    QImage aux = result;
    Dilate(aux,size);
}

void imgProc::Closing(QImage& buf, int size){
    Dilate(buf,size);
    QImage aux = result;
    Erode(aux,size);
}

void imgProc::TopHat(QImage& buf, int size){
    Opening(buf,size);
    for(int i = 0;i<source.width();i++)
        for(int j = 0;j<source.height();j++){
            int R = source.pixelColor(i,j).red()-result.pixelColor(i,j).red();
            int G = source.pixelColor(i,j).green()-result.pixelColor(i,j).green();
            int B = source.pixelColor(i,j).blue()-result.pixelColor(i,j).blue();
            if(R<0) R = 0;
            if(G<0) G = 0;
            if(B<0) B = 0;
            result.setPixelColor(i,j,QColor(R,G,B));
        }
}

void imgProc::BlackHat(QImage& buf, int size){
    Closing(buf,size);
    for(int i = 0;i<source.width();i++)
        for(int j = 0;j<source.height();j++){
            int R = result.pixelColor(i,j).red()-source.pixelColor(i,j).red();
            int G = result.pixelColor(i,j).green()-source.pixelColor(i,j).green();
            int B = result.pixelColor(i,j).blue()-source.pixelColor(i,j).blue();
            if(R<0) R = 0;
            if(G<0) G = 0;
            if(B<0) B = 0;
            result.setPixelColor(i,j,QColor(R,G,B));
        }
}

void imgProc::MorphEdges(QImage& buf, int size){
    Erode(buf,size);
    for(int i = 0;i<buf.width();i++)
        for(int j = 0;j<buf.height();j++){
            int R = buf.pixelColor(i,j).red()-result.pixelColor(i,j).red();
            int G = buf.pixelColor(i,j).green()-result.pixelColor(i,j).green();
            int B = buf.pixelColor(i,j).blue()-result.pixelColor(i,j).blue();
            if(R<0) R = 0;
            if(G<0) G = 0;
            if(B<0) B = 0;
            result.setPixelColor(i,j,QColor(R,G,B));
        }
}


void imgProc::SharpenFilter(){
    QVector<QVector<double>> kernel({{-1,-1,-1},
                                  {-1, 9,-1},
                                  {-1,-1,-1}});
    result = source;
    ApplyKernel(source,result,kernel);
}

void imgProc::EmbossFilter(){
    QVector<QVector<double>> kernel({{-1,-1,0},
                                  {-1, 0,1},
                                  { 0, 1,1}});
    result = source;
    ApplyKernel(source,result,kernel,128);
}

void imgProc::Invert(){
     result = source;
     for(int i = 0; i<source.width(); i++)
         for(int j = 0; j<source.height(); j++) {
         result.setPixelColor(i,j,QColor(255-source.pixelColor(i,j).red(),255-source.pixelColor(i,j).green(),255-source.pixelColor(i,j).blue()));
         }
}


int imgProc::GetThr(QVector<int>& hist){
      int weihsum = 0;
      int hsum = 0;

      for (int t = 0; t < 256; t++)
      {
        weihsum += t * hist[t];
        hsum += hist[t];
      }

      double maxSigma = -1;
      int threshold = 0;
      int alpha1 = 0;
      int beta1 = 0;

      for (int t = 0; t < 256; t++)
      {
        alpha1 += t * hist[t];
        beta1 += hist[t];
        double w1 = (double)beta1 / hsum;
        double a = (double)alpha1 / beta1 - (double)(weihsum - alpha1) / (hsum - beta1);
        double sigma = w1 * (1 - w1) * a * a;
        if (sigma > maxSigma)
        {
          maxSigma = sigma;
          threshold = t;
        }
      }
      return threshold;
}

void imgProc::ApplyKernel(QImage& src, QImage& res,QVector<QVector<double>>& kernel, double bias){
    int size = kernel.size();

    for(int i = size/2; i<src.width()-size/2; i++)
        for(int j = size/2; j<src.height()-size/2; j++) {
            QVector<double> final(3);
            for(int s = i-size/2;s<i+size/2+1;s++)
                for(int t = j-size/2;t<j+size/2+1;t++)
                {
                    final[0] += src.pixelColor(s,t).red()*kernel[s-i+size/2][t-j+size/2];
                    final[1] += src.pixelColor(s,t).green()*kernel[s-i+size/2][t-j+size/2];
                    final[2] += src.pixelColor(s,t).blue()*kernel[s-i+size/2][t-j+size/2];
                }
            int R = (int)round(final[0] + bias);
            int G = (int)round(final[1] + bias);
            int B = (int)round(final[2] + bias);
            if(final[0]<0) final[0] = 0;
            if(final[0]>255) final[0] = 255;
            if(final[1]<0) final[1] = 0;
            if(final[1]>255) final[1] = 255;
            if(final[2]<0) final[2] = 0;
            if(final[2]>255) final[2] = 255;
            res.setPixelColor(i,j,QColor(R,G,B));
        }
}
