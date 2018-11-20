//
// Created by CC000033 on 2018/8/6.
//

#include "IResample.h"

void IResample::Update(XData data) {
    XData d = this->Resample(data);
    if (d.size > 0) {
        this->Notify(d);
    }
}
