#include "launcher/launcher_model.h"

namespace vsun::launcher {

void LauncherModel::set_selected_index(int idx)
{
    if(idx < 0) idx = 0;
    selected_index_ = idx;
}

void LauncherModel::move_left()
{
    if(selected_index_ > 0) selected_index_--;
}

void LauncherModel::move_right(int max_index)
{
    if(selected_index_ < max_index) selected_index_++;
}

} // namespace vsun::launcher
