#include <QImage>

class imgProc
{

public:
    QImage source;
    QImage result;
    imgProc() {}
    void setSource(QImage load) {
        result = source = load;
    }

    //Алгоритмы перевода в оттенки серого
    void toGray(int method);

    //Алгоритмы бинаризации
    void ThBinarize(int threshold, bool isAllChannels);
    void OtsuBinarize(bool isAllChannels);

    //Алгоритмы удаления шума
    void MedianBlur(int size);
    void GaussianBlur(int size);
    void BoxBlur(int size);

    //Алгоритмы выделения границ
    void SobelEdges();
    void RobertsEdges();
    void PrewittEdges();

    //Алгоритмы морфологической фильтрации
    void Dilate(QImage buf, int size);
    void Erode(QImage& buf, int size);
    void Opening(QImage& buf, int size);
    void Closing(QImage& buf, int size);
    void TopHat(QImage& buf, int size);
    void BlackHat(QImage& buf, int size);
    void MorphEdges(QImage& buf, int size);

    //Фильтры общего вида
    void SharpenFilter();
    void EmbossFilter();
    void Invert();

private:
    int GetThr(QVector<int>& hist);
    void ApplyKernel(QImage& src, QImage& res,QVector<QVector<double>>& kernel, double bias = 0);
};
