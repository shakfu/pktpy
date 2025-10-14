# Wrapping Status

## Currently Wrapped

- Core: Symbol, Atom, AtomArray, Object, Dictionary, Hashtab, Linklist
- UI/Patcher: Patcher, Box, Patchline
- Timing: Clock, defer/defer_low
- I/O: Outlet, Buffer
- Utilities: gensym, parse, post, error, object registration/notification


## To Wrap

1. [x] Inlet API (ext_inlets.h)

Critical gap - you have outlets but not inlets:
- inlet_new() - Create custom inlets
- inlet_assist() - Set inlet assist strings
- Related: outlet_assist() for your Outlet class

2. [x] Table/Array (ext_table.h)

Alternative to Buffer for non-audio data:
- table_new(), table_get(), table_set()
- Lighter than buffer for general numeric arrays

3. Qelem (ext_qelem.h)

Queue-based defer alternative:
- qelem_new(), qelem_set(), qelem_unset()
- More control than defer for UI updates

4. Systhread (ext_systhread.h)

Thread management (use cautiously):
- systhread_create(), systhread_join()
- systhread_mutex_new(), lock/unlock
- Enables proper Python threading

5. [x] Path/File (ext_path.h)

Already partially used but not exposed:
- path_createsysfile(), path_opensysfile()
- path_nameconform(), locatefile_extended()
- Essential for file I/O in Python

6. Preset/Pattr (ext_preset.h, ext_pattr.h)

State management integration:
- pattr_register(), pattr_unregister()
- Exposes Python state to Max's preset system

7. [x] Database (ext_database.h)

SQLite access:
- db_open(), db_query(), db_result_*
- Direct database operations from Python

8. Wind (ext_wind.h)

Advanced window management:
- wind_advise() for window events
- Complements existing Patcher API

9. Time (ext_itm.h)

Transport/timing beyond Clock:
- itm_parse() - Parse time strings
- itm_format() - Format time values
- time_* functions for tempo-aware scheduling

10. Message Sending

Currently missing direct message dispatch:
- typedmess() - Send arbitrary messages to objects
- object_method_typed() - More versatile than current Object.method()

## Priority Ranking

Essential (fill gaps):
1. Inlet - Completes I/O story
2. Path/File - Enable proper file handling
3. typedmess/object_method_typed - More flexible object communication

High value:
4. Qelem - Better UI thread control
5. Systhread - Enable true multithreading
6. Time/ITM - Tempo-aware operations

Nice to have:
7. Table - Lighter data arrays
8. Preset/Pattr - State management
9. Database - SQLite integration
10. Wind - Advanced window control

The most glaring omission is Inlet - asymmetric to have outlets without
inlets. Path/File would also significantly enhance Python's file I/O
capabilities beyond the basic execfile().
