#include "population.h"
#include <algorithm>
#ifdef OPEN_CV
    #include <opencv2/opencv.hpp>
#endif 
#include "lookup.h"
#include <chrono>

void population::update() {
    
    for (int i = 0; i < generations; ++i) { 
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Generation " << i << " of " << generations << '\n';
        for (int reps = 0; reps < replicates; ++reps) {
            std::shared_ptr<Lookup> lookup_pred = std::make_shared<Lookup>(preds_pop);
            std::shared_ptr<Lookup> lookup_prey = std::make_shared<Lookup>(prey_pop);
            
            time = timeTicks;
            while (--time) {
               #ifdef OPEN_CV
                    {
                        int drawX = 1000; //draws between 1000 1000 and 2000 2000
                        int drawY = 1000;
                        int size = 1000;
                        cv::Mat screen(size, size, CV_8UC3, cv::Scalar(0xff, 0xff, 0xff));

                        for (int prey_i = reps * prey_pop_count; prey_i < prey_pop_count * (reps + 1); ++prey_i) { 
                            if (prey_pop[prey_i]->alive) {
                                if (prey_pop[prey_i]->x > drawX && prey_pop[prey_i]->x < drawX + size) {
                                    if (prey_pop[prey_i]->y > drawY && prey_pop[prey_i]->y < drawY + size) {
                                        cv::circle(screen, cv::Point(prey_pop[prey_i]->x - drawX, prey_pop[prey_i]->y - drawY), 4, cv::Scalar(255, 0, 0));
                                    }
                                }
                            }
                        }
                        for (int pred_i = reps * predator_pop_count; pred_i < (predator_pop_count * (reps + 1)) ; ++pred_i) {
                            if (preds_pop[pred_i]->x > drawX && preds_pop[pred_i]->x < drawX + size) {
                                if (preds_pop[pred_i]->y > drawY && preds_pop[pred_i]->y < drawY + size) {
                                    cv::circle(screen, cv::Point(preds_pop[pred_i]->x - drawX, preds_pop[pred_i]->y - drawY), 8, cv::Scalar(0, 0, 255));
                                }
                            }
                        }
                        cv::imshow(windowName, screen);
                        cv::waitKey(1);
                    }
                #endif       
                //N * N loop over pred and prey to update them
                for (int prey_i = reps * prey_pop_count; prey_i < prey_pop_count * (reps + 1); ++prey_i) { 
                    prey_pop[prey_i]->reset();
                }
                for (int pred_i = reps * predator_pop_count; pred_i < (predator_pop_count * (reps + 1)) ; ++pred_i) {
                    preds_pop[pred_i]->reset();
                    for (int prey_i = reps * prey_pop_count; prey_i < prey_pop_count * (reps + 1); ++prey_i) { 

                        if (prey_pop[prey_i]->alive) {
                            prey_pop[prey_i]->getNearestAgentPrey(preds_pop[pred_i]);
                            preds_pop[pred_i]->getNearestAgentPred(prey_pop[prey_i]);
                        }
                    }
                    std::shared_ptr<agent> tmp = lookup_prey->valid_agent(preds_pop[pred_i]);

                   if ((preds_pop[pred_i]->input_agent[0])) {
                        std::cout << preds_pop[pred_i]->input_agent[0]->x << " ";
                    } else {
                        std::cout << "NA" << " ";
                    }

                    if (tmp) {
                        std::cout << tmp->x << std::endl;
                    } else {
                        std::cout << "NA" << std::endl;
                    }
                    if ((preds_pop[pred_i]->input_agent[0])) {
                        std::cout << preds_pop[pred_i]->input_agent[0]->y << " ";
                    } else {
                        std::cout << "NA" << " ";
                    }

                    if (tmp) {
                        std::cout << tmp->y << std::endl;
                    } else {
                        std::cout << "NA" << std::endl;
                    }
 
                    int lookupX = lookup_pred->getLocX(preds_pop[pred_i]);
                    int lookupY = lookup_pred->getLocY(preds_pop[pred_i]);
                    preds_pop[pred_i]->updatePred();
                    preds_pop[pred_i]->consume(timeTicks - time);
                    if (lookupX != lookup_pred->getLocX(preds_pop[pred_i]) || lookupY != lookup_pred->getLocY(preds_pop[pred_i])) {
                        lookup_pred->update(preds_pop[pred_i], lookupX, lookupY);
                    }
                }
                //N loop over prey to update them 
                for (int prey_i = reps * prey_pop_count; prey_i < prey_pop_count * (reps + 1); ++prey_i) { 
                    int lookupX = lookup_prey->getLocX(prey_pop[prey_i]);
                    int lookupY = lookup_prey->getLocY(prey_pop[prey_i]);
                    if (prey_pop[prey_i]->alive) {
                        prey_pop[prey_i]->updatePrey();
                        if (lookupX != lookup_prey->getLocX(prey_pop[prey_i]) || lookupY != lookup_prey->getLocY(prey_pop[prey_i])) {
                            lookup_prey->update(prey_pop[prey_i], lookupX, lookupY);
                        }
                    }
                }
            } //end of time
        } //end of replicates
        /*Pred reproduce*/ 
        {
            std::vector< std::shared_ptr<chrome> > pred_gametes;
            for (int pred_i = 0; pred_i < predator_pop_count * replicates ; ++pred_i) {
                int val = preds_pop[pred_i]->calcFitnessPred();
                for (int i = 0; i < val; ++i) {
                    pred_gametes.push_back(preds_pop[pred_i]->getGamete());
                }
                preds_pop[pred_i]->output_data(output_file_pred, false, i, pred_i);
            }
            reproduce(pred_gametes, preds_pop, predator_pop_count);
        } //used to score and free pred gametes after
        /*Prey reproduce*/
        {
            std::vector< std::shared_ptr<chrome> > prey_gametes;
            for (int prey_i = 0; prey_i < prey_pop_count * replicates ; ++prey_i) {
                int val = prey_pop[prey_i]->calcFitnessPrey();
                for (int i = 0; i < val; ++i) {
                    prey_gametes.push_back(prey_pop[prey_i]->getGamete());
                }
                prey_pop[prey_i]->output_data(output_file_prey, true, i, prey_i);
            }
            reproduce(prey_gametes, prey_pop, prey_pop_count);
        } //used to scope and free prey gametes after usage

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> timeTaken = end - start;
        std::cout << timeTaken.count()/1000.0 << " Seconds Taken For generation\n";
    } //end of generations

}

template<std::size_t SIZE>
void population::reproduce(std::vector< std::shared_ptr<chrome> > &gametes, std::array< std::shared_ptr<agent>, SIZE > &pop, int pop_size) {
    //if it is too small just totally replace it
    int gametes_select = 0;
    int gametes_size = gametes.size();

    std::random_shuffle(gametes.begin(), gametes.end());
    std::cout << gametes_size << '\n';
    for (size_t i = 0; i < pop_size * replicates; ++i) {
        /*std::shared_ptr<chrome> p1 = gametes.back();
        gametes.pop_back();
        std::shared_ptr<chrome> p2 = gametes.back();
        gametes.pop_back();*/
        if (gametes_size == 0) {
            pop[i] = std::move(std::make_shared<agent>());
        } else {
            pop[i] = std::move(std::make_shared<agent>(gametes[gametes_select % gametes_size], gametes[(gametes_select + 1) % gametes_size]));
            gametes_select += 2;
        }
        if (gametes_select > gametes_select) {
            gametes_select = 0;
            std::random_shuffle(gametes.begin(), gametes.end());
        }
    }
}

