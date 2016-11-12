#ifndef ENUMS_H
#define ENUMS_H

// ActiveState contains a state value
// that reflects whether the unit is currently active or not.
enum SRV_STATUS {
    INACTIVE = -1,
    ACTIVE,
    FAILED,
    ACTIVATING,
    DEACTIVATING,
    RELOADING
};

#endif // ENUMS_H
