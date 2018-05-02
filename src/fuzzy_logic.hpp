#ifndef FUZZY_LOGIC_H
#define FUZZY_LOGIC_H
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <string>

/* ==== Fuzzy logic classifier tutorial  ====
 * First you create a fuzzyClass and assign the points to the classifier
 * example:
 * |         (0.5)
 * |         /\
 * |        /  \
 * |_______/    \___________(1.0)
 * (0.0)  (0.3) (0.7)
 *
 * This curve is created by:
 * fuzzyClass warm(0.0,0.0);  // The end points
 * warm.add_point(0.3,0.0);
 * warm.add_point(0.5,0.1);  // The midd point
 * warm.add_point(0.7,0.0)
 * Then we create the fussyLogic object.
 * fussyLogic weather;
 * weather.add_classifier('warm', &warm);
 * To run the classifier we need some ware to store the data therfore:
 * std::vector<float> values;
 * weather.classify(&values);
 * Thus then values contain the fuzzy results;
 */



class fuzzyClass
{
public:
   fuzzyClass (float begin_value, float end_value);
   virtual ~fuzzyClass (){};
   void add_point(float x, float y);
   float getLevel(float x);
   void show_points();
private:
   void sort();
   std::vector<std::vector<float>> points;
   int _nr_of_points;
};

class fuzzyLogic
{
private:
   std::vector<fuzzyClass*> _fuzzy;
   std::vector<std::string> _labels;

public:
   fuzzyLogic();
   virtual ~fuzzyLogic(){};
   void add_classifier(std::string label, fuzzyClass *fuzzy);
   int classyfy(float x, std::vector<float> *results);
   int classyfy_label(std::vector<std::string> *results);
};

#endif /* FUZZY_LOGIC_H */
