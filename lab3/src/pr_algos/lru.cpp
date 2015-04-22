class LRU:public AbstractPR{
  private:
  public:
    LRU(vector<pte>* pt, vector<unsigned int>* ft, vector<unsigned int>* ftop): AbstractPR(pt, ft, ftop){
    }

    void update_counter(unsigned int frame_number){
      for(int i = 0; i < frame_table->size(); i++){
        if(frame_table->at(i) == frame_number){
          frame_table->erase(frame_table->begin() + i);
          break;
        }
      }
      frame_table->push_back(frame_number);
    }

    int get_frame_to_replace(){
      unsigned int physical_frame = frame_table->front();
      frame_table->erase(frame_table->begin());
      frame_table->push_back(physical_frame);
      return physical_frame;
    }
};

