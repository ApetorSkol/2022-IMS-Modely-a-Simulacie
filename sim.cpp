#include <iostream>
#include "simlib.h"

using namespace std;


int Attack_warheads;
int Attack_decoy;

int Defence_warheads;

Stat success_track("P(track) when defending side succeded");
Stat fail_track("P(track) when defending side failed");
Stat fail_classify("Number of badly clasified real missiles");

double track;
double min_track;

int fail;
int success;


int count = 0;
int attack_count = 0;
double all_tracks = 0;

class AttackAndDefence : public Process {
    void Behavior() {    
        count++;
        attack:
        attack_count++;
        track = Uniform(9940,10000)/10000;
        all_tracks += track;
        
        if(Defence_warheads < 4 && Attack_warheads > 0){
            fail++;
            fail_track(all_tracks/attack_count);
        }
        else if(Attack_warheads > 0){
            
            if(Attack_decoy > 0){
                int missile_type = int(Uniform(0, Attack_decoy + Attack_warheads));
                
                if(missile_type < Attack_warheads){
                    Attack_warheads--;
                    double discriminant = Normal(6, 0.8);
                    
                    if(discriminant < 3.8){
                        fail++;     
                        fail_classify(discriminant); 
                    }
                    else{
                        Defence_warheads -= 4;
                        double defended = Random();
                        if(defended > track){
                            fail++;
                            fail_track(all_tracks/attack_count);
                        }
                        else{
                            goto attack;
                        }
                    }
                }
                else{
                    Attack_decoy--;
                    double discriminant = Normal(4, 0.8);
                    if(discriminant > 3.8){
                        Defence_warheads -= 4;
                        
                    }
                    goto attack;
                }
            }
            else{
                Attack_warheads--;
                double discriminant = Normal(6, 0.8);
                Defence_warheads -= 4;
                if(discriminant < 3.8){
                    fail++;
                    fail_classify(discriminant);
                }
                else{
                    Defence_warheads -= 4;
                    double defended = Random();
                    if(defended > track){
                        fail++;
                        fail_track(all_tracks/attack_count);
                    }
                    else{
                        goto attack;
                    }
                }                  
            }
        }
        else{
            success++;
            success_track(all_tracks/attack_count);
        }
        
    } 
};

class Battle : public Event {
    void Behavior() {
        
        Attack_warheads = int(Uniform(50, 200));
        Attack_decoy = int(Attack_warheads * 2.5);
        
        Defence_warheads = 100 + 1200 + 650;
        
        (new AttackAndDefence)->Activate();
    }
};


int main() {
    RandomSeed(time(NULL));
    
    fail = 0;
    success = 0;

    for(int i = 0; i < 100000; i++){
        
        all_tracks = 0;
        attack_count = 0;

        Init(0,10000);
        (new Battle)->Activate();
        Run();
        
        
    }
    cout << "Number of battles:  " << count << "\n";
    cout << "Defence failed:  " << fail << "\n";
    cout << "Defence succeded:  " << success << "\n";    
    success_track.Output();
    fail_track.Output();
    fail_classify.Output();

  return 0;
}