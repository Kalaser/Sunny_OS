#pragma once

#include <cstdint>

namespace vsun::launcher {

class LauncherModel {
public:
    int selected_index() const { return selected_index_; }

    void set_selected_index(int idx);
    void move_left();
    void move_right(int max_index);

private:
    int selected_index_ = 1;
};

} // namespace vsun::launcher

