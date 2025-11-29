#ifndef TOUCHED_H
#define TOUCHED_H

/*
    检查当前的commit之前是否有过add或者rm
*/

#pragma once
namespace touched {
    void set();
    bool get();
    void reset();
}

#endif // TOUCHED_H