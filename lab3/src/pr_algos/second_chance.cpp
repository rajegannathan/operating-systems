#include <iostream>
class SecondChance:public AbstractPR{
  private:
  public:
    SecondChance(vector<pte>* pt, vector<unsigned int>* ft, vector<unsigned int>* ftop): AbstractPR(pt, ft, ftop){
    }

    void update_counter(unsigned int frame_number){
    }

    int get_frame_to_replace(){
      unsigned int physical_frame;
      unsigned int pt_index;

      for(int position = 0; position < frame_table->size(); position++){
        physical_frame = frame_table->at(0);
        pt_index = ftop->at(physical_frame);

        if(page_table->at(pt_index).referenced == 0){
          break;
        }
        //cout << "resetting reference in "<< pt_index << endl;
        page_table->at(pt_index).referenced = 0;

        frame_table->erase(frame_table->begin());
        frame_table->push_back(physical_frame);
      }

      physical_frame = frame_table->at(0);
      frame_table->erase(frame_table->begin());
      frame_table->push_back(physical_frame);

      return physical_frame;
    }
};

