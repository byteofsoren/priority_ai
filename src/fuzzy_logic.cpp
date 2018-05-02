#include "fuzzy_logic.hpp"
#include <assert.h>


fuzzyClass::fuzzyClass(float begin_value, float end_value) {
   add_point(0.0,begin_value);
   add_point(1.0,end_value);
   _nr_of_points = 1;
}

void fuzzyClass::add_point(float x, float y) {
   //MESSAGE("fuzzyClass","add_point");
   std::vector<float> row;
   row.push_back(x);
   row.push_back(y);
   points.push_back(row);
   _nr_of_points++;
   sort();
}

float fuzzyClass::getLevel(float x) {
   int low = 0;
   int high = 0;
   if (x <= 0.0) {
      return points[0][1];
   }
   if (x >= 1.0) {
      return points[_nr_of_points][1];
   }
   for (size_t i = 0; i < points.size(); ++i) {
      if (points[i][0] >= x) {
         low = i - 1;
         break;
      }
   }
   high = low + 1;
   /* Calculating the line between each point is done
    *    y = kx + m
    *    k = -  y_2 - y_1 / x_2 - x_1
    *    m = y_1 - k*x_1
    */
   float k =(points[high][1] - points[low][1])/(points[high][0] - points[low][0]);
   float m = points[low][1] - k * points[low][0];
   float kxm = k*x + m;
   return kxm;
}

bool _sort_comp(const std::vector<float> v1, const std::vector<float> v2){
   return v1[0] < v2[0];
}

void fuzzyClass::sort() {
   //std::sort(points[0].begin(), points[0].end());
   std::sort(points.begin(), points.end(), _sort_comp);
}

void fuzzyClass::show_points() {
   for (size_t i = 0; i < points.size(); ++i) {
      printf("x=%0.2f, y=%0.2f\n",points[i][0],points[i][1] );
   }
}
// ============== fuzzyLogic ==========================

fuzzyLogic::fuzzyLogic() {
}

void fuzzyLogic::add_classifier(std::string label, fuzzyClass *fuzzy) {
   _fuzzy.push_back(fuzzy);
   _labels.push_back(label);
}

int fuzzyLogic::classyfy(float x,std::vector<float> *results) {
   int couter = 0;
   for (size_t i = 0; i < _fuzzy.size(); ++i) {
      results->push_back(_fuzzy[i]->getLevel(x));
      couter++;
   }
   return couter;
}

int fuzzyLogic::classyfy_label(std::vector<std::string> *results) {
   int couter = 0;
   for (size_t i = 0; i < _fuzzy.size(); ++i) {
      results->push_back(_labels[i]);
   }
   return couter;
}
