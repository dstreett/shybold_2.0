#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include "agents.h"

#define PI 3.14159

void agent::updatePrey() {
    std::vector<double> inputs;
    if (input_agent.size() != 0 && input_agent[0]) { //change this (loop througth input_agents and push_back onto inputs
        saw_last = true;
        inputs = { this->distance(input_agent[0]),  atan2(input_agent[0]->y - y, input_agent[0]->x - x), 0.0, 1.0 };
    } else {
        //make appropariate number of inputs
        saw_last = false;
        inputs = { 0, 0, getRandom(), 1.0 };
    }
    //you hand it chrome 1 values and chrome 2 values from genome
    n->update(g, inputs);
    //move based on ouptputs
    move_x_y(n->output_values[0] * pred_capture,  n->output_values[1] * pred_capture);
    //move_mag_theta(n->output_values[0], n->output_values[1], n->output_values[2]);
    //move_mag_theta(n->output_values[0], n->output_values[1], 0.0, pred_capture);
    


}

void agent::updatePred(int time) {
    std::vector<double> inputs;
    double tmp_x = x;
    double tmp_y = y;
    if (input_agent.size() != 0 && input_agent[0]) { //change this (loop througth input_agents and push_back onto inputs
        inputs = { this->distance(input_agent[0]),  atan2(input_agent[0]->y - y, input_agent[0]->x - x), 0.0, 1.0 };
    } else {
        inputs = { 0, 0, getRandom(), 1.0 };
    }
    //you hand it chrome 1 values and chrome 2 values from genome
    n->update(g, inputs);
    move_mag_theta(n->output_values[0], n->output_values[1], 0.0, pred_capture);
    

    double xdif = x - tmp_x;
    double ydif = y - tmp_y;

    sum_x_step += xdif;
    sum_y_step += ydif;
    sq_sum_x_step += xdif * xdif;
    sq_sum_y_step += ydif * ydif;
    
    calc_stuff();
    consume(time);
    chance_of_death(time);

}

//Need to change to get sorted vector of agents inputs
void agent::getNearestAgentPrey(const std::shared_ptr<agent> &a) {

   if (a->alive && valid_agent(a)) {
       input_agent[0] = a;
    } 
    /*
    if (input_agent.size() == 0) {
        if (this->distance(a) < sensing_range_prey ) {
            input_agent.push_back(a);
        }
    } else if (!input_agent[0] || this->distance(a) < this->distance(input_agent[0])) {
        //change it here, sort the input_agents and push them appropriately on
        if (this->distance(a) < sensing_range_prey) {
            input_agent[0] = a;
        }
    }*/
}

void agent::getNearestAgentPred(const std::shared_ptr<agent> &a) {
   if (valid_agent(a)) {
        input_agent[0] = a;
    }  
   /*if (input_agent.size() == 0) {
        if (this->distance(a) < sensing_range_pred) {
            input_agent.push_back(a);
        }
    } else if ( !input_agent[0] || this->distance(a) < this->distance(input_agent[0]) || !input_agent[0]->alive ) {
        //same thing here
        if (this->distance(a) < sensing_range_pred) {
            input_agent[0] = a;
        }
    }*/
}

//figure out what you want to do here
void agent::consume(int time) {
    if (input_agent[0]) {
        if (this->distance(input_agent[0]) < pred_capture) {
            input_agent[0]->alive = false;
            input_agent[0]->lastTime = time;
            ++fitness;
        }
        /*if (this->distance(input_agent[0]) < pred_capture && input_agent[0]->alive) {
            input_agent[0]->alive = false;
            input_agent[0]->lastTime = time;
            ++fitness;
        }*/
    }
}

void agent::move_x_y(double dx, double dy) {
    x = std::max(0.0, std::min(x + dx, sizeX));
    y = std::max(0.0, std::min(y + dy, sizeY));
}

void agent::move_mag_theta(double mag, double theta, double direction_facing, double move) {
    /*std::cout << "X " << sin(theta) * mag << '\n';
    std::cout << "Y " << cos(theta) * mag << '\n';
    std::cout << "Theta " << theta << '\n';
    std::cout << "Mag " << mag << '\n';*/
    x = std::max(0.0, std::min(cos(theta) * mag * move + x, sizeX));
    y = std::max(0.0, std::min(sin(theta) * mag * move + y, sizeY));
    //angle_facing = std::fmod(angle_facing + direction_facing, 2 * PI);
    
}

void agent::output_data(std::fstream &o, bool prey, int gen, int index) {
    o << gen << '\t';
    o << idTrue << '\t';
    if (prey) {
        o << lastTime << '\t';
    } else {
        o << fitness << '\t';
    }
    
    for (int i = 0; i < geneNN; ++i) {
        o << g->getWeight(i) << '\t';
    }

    o << ((g->c1)->metabolic + (g->c2)->metabolic)/2.0 << '\t';
    o << ((g->c1)->radius + (g->c2)->radius) /2.0 << '\t';
    o << g->getStddev() << '\t'; 
    if (prey) {
        o << calcFitnessPrey() << '\t';
    } else {
        o << calcFitnessPred() << '\t';
    }
    o << 
        sum_x / deathtime << "\t" << sum_y / deathtime << "\t" << 
        sq_sum_x / deathtime - (sum_x /deathtime) * (sum_x/deathtime) << '\t' << 
        sq_sum_y / deathtime - (sum_y /deathtime) * (sum_y/deathtime) << '\t' << 
        sum_x_step / deathtime << "\t" << sum_y_step / deathtime << "\t" << 
        sq_sum_x_step / deathtime - (sum_x_step /deathtime) * (sum_x_step/deathtime) << '\t' << 
        sq_sum_y_step / deathtime - (sum_y_step /deathtime) * (sum_y_step/deathtime) << '\t' << deathtime << std::endl;
}
