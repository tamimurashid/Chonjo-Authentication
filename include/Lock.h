#ifndef LOCK_H
#define LOCK_H

class Lock{
    public:
    Lock(int trigger, unsigned long backresponse );
    void Open();

    private:
    int trigger;
    unsigned long backresponse;
  
};

#endif //LOCK_H