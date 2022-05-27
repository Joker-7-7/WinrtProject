#ifndef VERTEX_H
#define VERTEX_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>



struct Vertex
{
    Vertex() {}
    Vertex(float x = 0.0, float y = 0.0, float z = 0.0) : x(x), y(y), z(z) {}
    Vertex(std::vector<float> v) : x(v[0]), y(v[1]), z(v[2]) {}

    float x, y, z;
};

bool operator ==(Vertex a, Vertex b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

struct Normal
{
    Normal() {}
    Normal(float x = 0.0, float y = 0.0, float z = 0.0) : x(x), y(y), z(z) {}
    Normal(std::vector<float> v) : x(v[0]), y(v[1]), z(v[2]) {}
    XMVECTOR getVec()
    {
        return XMVectorSet(x, y, z, 1.0f);;
    }

    float x, y, z;
};

struct Triangle
{
    Vertex vert_1, vert_2, vert_3;
    Normal norm;
    Triangle(Vertex vertTmp_1, Vertex vertTmp_2, Vertex vertTmp_3, Normal normTmp)
        :vert_1(vertTmp_1), vert_2(vertTmp_2), vert_3(vertTmp_3), norm(normTmp)
    {
    }
    void setNorm(Normal normTmp) { norm = normTmp; }
    void setVerts(Vertex vertTmp_1, Vertex vertTmp_2, Vertex vertTmp_3)
    {
        vert_1 = vertTmp_1;
        vert_2 = vertTmp_2;
        vert_3 = vertTmp_3;
    }
    XMVECTOR getNormVec()
    {
        return norm.getVec();
    }
};


struct Plane
{
    Vertex vert_1, vert_2, vert_3;
    Plane(Vertex vertTmp_1, Vertex vertTmp_2, Vertex vertTmp_3) : vert_1(vertTmp_1), vert_2(vertTmp_2), vert_3(vertTmp_3)
    {}
    XMVECTOR getNormal()
    {
        XMVECTOR vec1 = XMVectorSet(vert_1.x - vert_2.x, vert_1.y - vert_2.y, vert_1.z - vert_2.z, 1.0);
        XMVECTOR vec2 = XMVectorSet(vert_3.x - vert_2.x, vert_3.y - vert_2.y, vert_3.z - vert_2.z, 1.0f);
        return XMVector3Normalize(XMVector3Cross(vec1, vec2));
    }

};

void createTriangles(std::vector<Triangle>& triangles, std::vector<Vertex>& vertices, std::vector<Normal>& normals)
{

    for (size_t i = 0; i < normals.size(); ++i)
    {
        Triangle tmpTriangle(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], normals[i]);

        triangles.push_back(tmpTriangle);
    }
}

void getVerticesForIntersectionTriangles(std::vector<Triangle>& Triangles, std::vector<Vertex>& verts)
{
    for (size_t i = 0; i < Triangles.size(); ++i)
    {
        verts.push_back(Vertex(Triangles[i].vert_1.x,
            Triangles[i].vert_1.y,
            Triangles[i].vert_1.z));
        verts.push_back(Vertex(Triangles[i].vert_2.x,
            Triangles[i].vert_2.y,
            Triangles[i].vert_2.z));
        verts.push_back(Vertex(Triangles[i].vert_3.x,
            Triangles[i].vert_3.y,
            Triangles[i].vert_3.z));
    }
}
float dotProduct(XMVECTOR a, XMVECTOR b)
{
    return XMVectorGetX(a) * XMVectorGetX(b) + XMVectorGetY(a) * XMVectorGetY(b) + XMVectorGetZ(a) * XMVectorGetZ(b);
}
void differentiationTriangles(std::vector<Triangle>& intersectionTriangles, std::vector<Triangle>& newTriangles, Plane& plane, int revert = 1)
{

    XMVECTOR planeNormal = -plane.getNormal();
    planeNormal *= revert;
    Vertex pointOnPlane = plane.vert_1;
    std::vector <Vertex> intersectionPoints;
    for (size_t i = 0; i < intersectionTriangles.size(); ++i)
    {
        XMVECTOR dP1 = XMVectorSet(pointOnPlane.x - intersectionTriangles[i].vert_1.x,
            pointOnPlane.y - intersectionTriangles[i].vert_1.y,
            pointOnPlane.z - intersectionTriangles[i].vert_1.z, 1.0f);
        XMVECTOR dP2 = XMVectorSet(pointOnPlane.x - intersectionTriangles[i].vert_2.x,
            pointOnPlane.y - intersectionTriangles[i].vert_2.y,
            pointOnPlane.z - intersectionTriangles[i].vert_2.z, 1.0f);
        XMVECTOR dP3 = XMVectorSet(pointOnPlane.x - intersectionTriangles[i].vert_3.x,
            pointOnPlane.y - intersectionTriangles[i].vert_3.y,
            pointOnPlane.z - intersectionTriangles[i].vert_3.z, 1.0f);

        XMVECTOR p1 = XMVectorSet(intersectionTriangles[i].vert_2.x - intersectionTriangles[i].vert_1.x,
            intersectionTriangles[i].vert_2.y - intersectionTriangles[i].vert_1.y,
            intersectionTriangles[i].vert_2.z - intersectionTriangles[i].vert_1.z, 1.0f);
        XMVECTOR p2 = XMVectorSet(intersectionTriangles[i].vert_3.x - intersectionTriangles[i].vert_2.x,
            intersectionTriangles[i].vert_3.y - intersectionTriangles[i].vert_2.y,
            intersectionTriangles[i].vert_3.z - intersectionTriangles[i].vert_2.z, 1.0f);
        XMVECTOR p3 = XMVectorSet(intersectionTriangles[i].vert_3.x - intersectionTriangles[i].vert_1.x,
            intersectionTriangles[i].vert_3.y - intersectionTriangles[i].vert_1.y,
            intersectionTriangles[i].vert_3.z - intersectionTriangles[i].vert_1.z,1.0f);

        float t1 = ((dotProduct(planeNormal, dP1)) / (dotProduct(planeNormal, p1)));
        float t2 = (dotProduct(planeNormal, dP2)) / (dotProduct(planeNormal, p2));
        float t3 = (dotProduct(planeNormal, dP3)) / (dotProduct(planeNormal, p3));

        std::vector<Vertex> o;
        o.push_back(Vertex(t1 * XMVectorGetX(p1) + intersectionTriangles[i].vert_1.x,
            t1 * XMVectorGetY(p1) + intersectionTriangles[i].vert_1.y,
            t1 * XMVectorGetZ(p1) + intersectionTriangles[i].vert_1.z));
        o.push_back(Vertex(t2 * XMVectorGetX(p2) + intersectionTriangles[i].vert_2.x,
            t2 * XMVectorGetY(p2) + intersectionTriangles[i].vert_2.y,
            t2 * XMVectorGetZ(p2) + intersectionTriangles[i].vert_2.z));
        o.push_back(Vertex(t3 * XMVectorGetX(p3) + intersectionTriangles[i].vert_3.x,
            t3 * XMVectorGetY(p3) + intersectionTriangles[i].vert_3.y,
            t3 * XMVectorGetZ(p3) + intersectionTriangles[i].vert_3.z));



        std::vector <Vertex> deletePoints;
        if (dotProduct(planeNormal, dP1) > 0)
            deletePoints.push_back(intersectionTriangles[i].vert_1);
        if (dotProduct(planeNormal, dP2) > 0)
            deletePoints.push_back(intersectionTriangles[i].vert_2);
        if (dotProduct(planeNormal, dP3) > 0)
            deletePoints.push_back(intersectionTriangles[i].vert_3);

        if (deletePoints.size() == 1)
        {
            if (deletePoints[0] == intersectionTriangles[i].vert_1)
            {
                newTriangles.push_back(Triangle(o[2], intersectionTriangles[i].vert_3, o[0], Normal(0.0, 0.0, 0.0)));
                newTriangles.push_back(Triangle(intersectionTriangles[i].vert_3, intersectionTriangles[i].vert_2, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[2]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_2)
            {
                newTriangles.push_back(Triangle(o[1], intersectionTriangles[i].vert_3, o[0], Normal(0.0, 0.0, 0.0)));
                newTriangles.push_back(Triangle(intersectionTriangles[i].vert_3, intersectionTriangles[i].vert_1, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[1]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_3)
            {
                newTriangles.push_back(Triangle(o[2], intersectionTriangles[i].vert_1, o[1], Normal(0.0, 0.0, 0.0)));
                newTriangles.push_back(Triangle(intersectionTriangles[i].vert_1, intersectionTriangles[i].vert_2, o[1], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[1]);
                intersectionPoints.push_back(o[2]);
            }
        }
        else if (deletePoints.size() == 2)
        {
            if (deletePoints[0] == intersectionTriangles[i].vert_2 && deletePoints[1] == intersectionTriangles[i].vert_3)
            {
                newTriangles.push_back(Triangle(o[2], intersectionTriangles[i].vert_1, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[2]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_1 && deletePoints[1] == intersectionTriangles[i].vert_3)
            {
                newTriangles.push_back(Triangle(o[1], intersectionTriangles[i].vert_2, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[1]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_1 && deletePoints[1] == intersectionTriangles[i].vert_2)
            {
                newTriangles.push_back(Triangle(o[1], intersectionTriangles[i].vert_3, o[2], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[1]);
                intersectionPoints.push_back(o[2]);
            }
        }

        //newTriangles.push_back(o[0]);
        //newTriangles.push_back(o[1]);
        //newTriangles.push_back(o[2]);
    }


}

void findIntersectionTriangles(std::vector<Triangle>& objectTriangles, std::vector<Triangle>& intersectionTriangles, Plane& plane)
{
    XMVECTOR planeNormal = plane.getNormal();
    Vertex pointOnPlane = plane.vert_1;

    for (size_t i = 0; i < objectTriangles.size(); ++i)
    {
        XMVECTOR tmpVec1 = XMVectorSet(objectTriangles[i].vert_1.x - pointOnPlane.x, objectTriangles[i].vert_1.y - pointOnPlane.y, objectTriangles[i].vert_1.z - pointOnPlane.z, 1.0f);
        XMVECTOR tmpVec2 = XMVectorSet(objectTriangles[i].vert_2.x - pointOnPlane.x, objectTriangles[i].vert_2.y - pointOnPlane.y, objectTriangles[i].vert_2.z - pointOnPlane.z, 1.0f);
        XMVECTOR tmpVec3 = XMVectorSet(objectTriangles[i].vert_3.x - pointOnPlane.x, objectTriangles[i].vert_3.y - pointOnPlane.y, objectTriangles[i].vert_3.z - pointOnPlane.z, 1.0f);

        if (!((dotProduct(tmpVec1, planeNormal) > 0 && dotProduct(tmpVec2, planeNormal) > 0 && dotProduct(tmpVec3, planeNormal) > 0)
            || (dotProduct(tmpVec1, planeNormal) < 0 && dotProduct(tmpVec2, planeNormal) < 0 && dotProduct(tmpVec3, planeNormal) < 0)))
            intersectionTriangles.push_back(objectTriangles[i]);

    }
}

void mergeTriangles(std::vector<Triangle>& firstArrayTriangles, std::vector<Triangle>& secondArrayTriangles)
{
    for (size_t i = 0; i < firstArrayTriangles.size(); ++i)
    {
        secondArrayTriangles.push_back(firstArrayTriangles[i]);
    }
}


void findTrianglesOneSide(std::vector<Triangle>& objectTriangles, std::vector<Triangle>& sideTriangles, Plane& planeFirst, Plane& planeSecond, int revert = 1)
{
    XMVECTOR planeNormalFirst = planeFirst.getNormal() * revert;
    Vertex pointOnPlaneFirst = planeFirst.vert_1;

    //XMVECTOR planeNormalSecond = -planeSecond.getNormal();
    //Vertex pointOnPlaneSecond = planeSecond.vert_1;

    for (size_t i = 0; i < objectTriangles.size(); ++i)
    {
        XMVECTOR tmpVec11 = XMVectorSet(objectTriangles[i].vert_1.x - pointOnPlaneFirst.x, objectTriangles[i].vert_1.y - pointOnPlaneFirst.y, objectTriangles[i].vert_1.z - pointOnPlaneFirst.z, 1.0f);
        XMVECTOR tmpVec21= XMVectorSet(objectTriangles[i].vert_2.x - pointOnPlaneFirst.x, objectTriangles[i].vert_2.y - pointOnPlaneFirst.y, objectTriangles[i].vert_2.z - pointOnPlaneFirst.z, 1.0f);
        XMVECTOR tmpVec31 = XMVectorSet(objectTriangles[i].vert_3.x - pointOnPlaneFirst.x, objectTriangles[i].vert_3.y - pointOnPlaneFirst.y, objectTriangles[i].vert_3.z - pointOnPlaneFirst.z, 1.0f);

        //XMVECTOR tmpVec12 = XMVectorSet(objectTriangles[i].vert_1.x - pointOnPlaneSecond.x, objectTriangles[i].vert_1.y - pointOnPlaneSecond.y, objectTriangles[i].vert_1.z - pointOnPlaneSecond.z, 1.0f);
        //XMVECTOR tmpVec22 = XMVectorSet(objectTriangles[i].vert_2.x - pointOnPlaneSecond.x, objectTriangles[i].vert_2.y - pointOnPlaneSecond.y, objectTriangles[i].vert_2.z - pointOnPlaneSecond.z, 1.0f);
        //XMVECTOR tmpVec32 = XMVectorSet(objectTriangles[i].vert_3.x - pointOnPlaneSecond.x, objectTriangles[i].vert_3.y - pointOnPlaneSecond.y, objectTriangles[i].vert_3.z - pointOnPlaneSecond.z, 1.0f);

        if ((dotProduct(tmpVec11, planeNormalFirst) > 0
            && dotProduct(tmpVec21, planeNormalFirst) > 0
            && dotProduct(tmpVec31, planeNormalFirst) > 0)
            
/*            &&(dotProduct(tmpVec12, planeNormalSecond) > 0
                && dotProduct(tmpVec22, planeNormalSecond) > 0
                && dotProduct(tmpVec32, planeNormalSecond) > 0)*/)
            sideTriangles.push_back(objectTriangles[i]);

    }


}


void sliceSolid(std::vector<Triangle>& objectTriangles, Plane& planeFirst, Plane& planeSecond, std::vector <Vertex>& verts)
{
    std::vector<Triangle> intersectionTrianglesWithFirstPlane;
    findIntersectionTriangles(objectTriangles, intersectionTrianglesWithFirstPlane, planeFirst);

    std::vector<Triangle> dontIntersectonTrianglesFirstPlane;
    findTrianglesOneSide(objectTriangles, dontIntersectonTrianglesFirstPlane, planeFirst, planeFirst);

    std::vector<Triangle> firstSliceTriangles;
    differentiationTriangles(intersectionTrianglesWithFirstPlane, firstSliceTriangles, planeFirst, -1);
    mergeTriangles(dontIntersectonTrianglesFirstPlane, firstSliceTriangles);

    std::vector<Triangle> intersectionTrianglesWithSecondPlane;
    findIntersectionTriangles(firstSliceTriangles, intersectionTrianglesWithSecondPlane, planeSecond);

    std::vector<Triangle> dontIntersectonTrianglesSecondPlane;
    findTrianglesOneSide(firstSliceTriangles, dontIntersectonTrianglesSecondPlane, planeSecond, planeSecond,-1);

    std::vector<Triangle> secondSliceTriangles;
    differentiationTriangles(intersectionTrianglesWithSecondPlane, secondSliceTriangles, planeSecond);
    mergeTriangles(dontIntersectonTrianglesSecondPlane, secondSliceTriangles);

    //mergeTriangles(firstSliceTriangles, secondSliceTriangles);
    
    getVerticesForIntersectionTriangles(secondSliceTriangles, verts);
}

int read_binary_STL_file(std::string STL_filename, std::vector<Triangle>& facet)
{
    // specify the location of STL files on this computer

    // declare an (input) file object
    std::ifstream binaryInputFile;

    // open the STL file by using the full path and the name
    // specify that the file is opened in "read-only" mode
    // specify that the file is opened in binary format
    binaryInputFile.open((STL_filename).c_str(), std::ifstream::in | std::ifstream::binary);

    // check whether the file was opened successfully
    // if yes then continue otherwise terminate program execution
    if (binaryInputFile.fail())
    {
        //std::cout << "ERROR: Input STL file could not be opened!" << std::endl;
        return (1);
    }

    // position the pointer to byte number 80
    binaryInputFile.seekg(80);

    // read the number of facets (triangles) in the STL geometry
    int numberOfTriangles;
    binaryInputFile.read((char*)&numberOfTriangles, sizeof(int));

    // declare an object "tri" of type triangle (see main.h for the definition of the triangle class)
    Triangle tri(Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Normal(0.0, 0.0, 0.0));

    // storage space for the "unused bytes" 
    char unused_bytes[2];


    // temporary floating point variable
    float temp_float_var;

    for (int count = 0; count < numberOfTriangles; count++)
    {
        // read the three components of the normal vector
        binaryInputFile.read((char*)&temp_float_var, 4); tri.norm.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.norm.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.norm.z = (double)temp_float_var;

        // read the three coordinates of vertex 1 
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_1.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_1.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_1.z = (double)temp_float_var;

        // read the three coordinates of vertex 2 
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_2.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_2.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_2.z = (double)temp_float_var;

        // read the three coordinates of vertex 3
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_3.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_3.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_3.z = (double)temp_float_var;

        // read the 2 unused bytes
        binaryInputFile.read(unused_bytes, 2);

        // add data for this triangle to the "facet" vector
        facet.push_back(tri);
    }

    // explicitly close the connection to the input STL file
    binaryInputFile.close();

    return (0);  // all is well
}

int read_ascii_STL_file(std::string STL_filename, std::vector<Triangle>& facet)
{
    // specify the location of STL files on this computer
    //std::string STL_files_path = "../stl/";

    // declare a (input) file object
    std::ifstream asciiInputFile;

    // open the STL file by using the full path and the name
    // specify that the file is opened in "read-only" mode
    asciiInputFile.open((/*STL_files_path  +*/STL_filename).c_str(), std::ifstream::in);

    // check whether the file was opened successfully
    // if yes then continue otherwise terminate program execution
    if (asciiInputFile.fail())
    {
        //std::cout << "ERROR: Input STL file could not be opened!" << std::endl;
        return(1); // error
    }

    // read in the contents line by line until the file ends

    // initialize counter for counting the number of lines in this file
    int triangle_number = 0;

    // declare an object "tri" of type triangle (see above for the definition of the triangle struct)
    Triangle tri(Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Normal(0.0, 0.0, 0.0));

    // declare some string objects
    std::string junk;
    std::string string1, string2;

    // read in the first line (until the /n delimiter) and store it in the string object "line"
    getline(asciiInputFile, junk);

    // begin loop to read the rest of the file until the file ends
    while (true)
    {
        // read the components of the normal vector
        asciiInputFile >> string1 >> string2 >> tri.norm.x >> tri.norm.y >> tri.norm.z;        //  1
        // continue reading this line until the \n delimiter
        getline(asciiInputFile, junk);                                                                //  1

        // read the next line until the \n delimiter
        getline(asciiInputFile, junk);                                                                //  2

        // read the (x,y,z) coordinates of vertex 1            
        asciiInputFile >> string1 >> tri.vert_1.x >> tri.vert_1.y >> tri.vert_1.z;             //  3
        getline(asciiInputFile, junk);                                                                //  3

        // read the (x,y,z) coordinates of vertex 2            
        asciiInputFile >> string1 >> tri.vert_2.x >> tri.vert_2.y >> tri.vert_2.z;             //  4
        getline(asciiInputFile, junk);                                                                //  4

        // read the (x,y,z) coordinates of vertex 3            
        asciiInputFile >> string1 >> tri.vert_3.x >> tri.vert_3.y >> tri.vert_3.z;             //  5
        getline(asciiInputFile, junk);                                                                //  5

        // read some more junk
        getline(asciiInputFile, junk);                                                                //  6
        getline(asciiInputFile, junk);                                                                //  7

        // break out of the while loop if "end-of-file" becomes true
        if (asciiInputFile.eof()) break;

        // increment the triangle number
        triangle_number++;

        // add data for this triangle to the "facet" vector
        facet.push_back(tri);
    }
    // end while loop

    // explicitly close the output file
    asciiInputFile.close();

    return (0);   // all is well
}

// this function handles the task of reading in input geometry files in STL format 
// and finding out the required size of the environmental box

int getGeometryInput(std::string STL_filename,
    std::vector<Triangle>& facet)
{
    // open the STL file and read its contents

    // specify the location of STL files on this computer
    //std::string STL_files_path = "../stl/";

    // BEGIN algorithm from ParaView to detect whether the STL file is ASCII or BINARY

    // open the STL file specified by the user as "read-only" and in "binary" mode
    // and store the pointer to the file buffer in "fp"
#pragma warning(suppress : 4996)
    FILE* fp = fopen((STL_filename).c_str(), "rb");

    // specify how many bytes of the file will be read to calculate the percentage of characters
    // that are non-ASCII
    unsigned long length = 256;

    // dynamically allocate a buffer for storing the contents of the file
    // based on the length specified above
    unsigned char* buffer = new unsigned char[length];

    // read an array of "length" elements from the STL file, where each element is 1 byte in size
    // store the result in the buffer defined above
    // fread returns the number of bytes successfully read and this is stored in "read_length" 
    size_t read_length = fread(buffer, 1, length, fp);

    // close the STL file
    fclose(fp);

    // loop over contents and count

    size_t text_count = 0;

    const unsigned char* ptr = buffer;
    const unsigned char* buffer_end = buffer + read_length;

    // loop over the entire buffer, starting at the memory location pointed to by "ptr"
    while (ptr != buffer_end)
    {
        if ((*ptr >= 0x20 && *ptr <= 0x7F) || *ptr == '\n' || *ptr == '\r' || *ptr == '\t')
        {
            text_count++;  // Yay! we have an ASCII character!
        }
        ptr++;    // move on to the next "byte"
    }

    // free memory
    delete[] buffer;

    // calculate the fraction of characters(bytes) in part of the STL file that are BINARY
    double current_percent_bin = (static_cast<double> (read_length - text_count) / static_cast<double> (read_length));

    // anything above the cut-off value below means that this STL file is of type BINARY
    double percent_bin = 0.05;

    std::string STL_filetype;

    if (current_percent_bin >= percent_bin)
    {
        STL_filetype = "binary";
    }
    else
    {
        STL_filetype = "ascii";
    }

    // END algorithm from ParaView to detect whether the STL file is ASCII or BINARY

    //std::cout << "Input Geometry File = " << STL_filename << " (" << STL_filetype << ")" << std::endl;

    // function call to open the ascii file and read the content
    if (STL_filetype == "ascii")
    {
        // call the function to read the ASCII file
        // if all goes well, this function will return a value of 0
        int error = read_ascii_STL_file(STL_filename, facet);

        // if something goes wrong, the above function will return a value of 1
        if (error == 1)
        {
            return(1);       // exit returning an error
        }
    }

    // function call to open the binary file and read the content
    if (STL_filetype == "binary")
    {
        // call the function to read the BINARY file
        // if all goes well, this function will return a value of 0
        int error = read_binary_STL_file(STL_filename, facet);

        // if something goes wrong, the above function will return a value of 1
        if (error == 1)
        {
            return(1);       // exit returning an error
        }
    }

    if (STL_filetype != "ascii" && STL_filetype != "binary")
    {
        //std::cout << "ERROR: Unable to find out whether the STL file is ascii or binary" << std::endl;
        return(1);
    }

    // print the number of triangles
    //std::cout << "The number of triangles in the STL file = " << facet.size() << std::endl;

    return (0);  // all is well
}



#endif