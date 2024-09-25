/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-12     latercomer   the first version
 */


#ifndef _UORB_H_
#define _UORB_H_


#include <rtthread.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**
 * Object metadata.
 */
struct orb_metadata
{
    const char    *o_name;            /**< unique object name */
    const uint16_t o_size;            /**< object size */
    const uint16_t o_size_no_padding; /**< object size w/o padding at the end (for logger) */
    const char    *o_fields;          /**< semicolon separated list of fields (with type) */
    uint8_t        o_id;              /**< ORB_ID enum */
};

typedef const struct orb_metadata *orb_id_t;


/**
 * Generates a pointer to the uORB metadata structure for
 * a given topic.
 *
 * The topic must have been declared previously in scope
 * with ORB_DECLARE().
 *
 * @param _name		The name of the topic.
 */
#define ORB_ID(_name) &__orb_##_name

/**
 * Declare (prototype) the uORB metadata for a topic (used by code generators).
 *
 * @param _name		The name of the topic.
 */
#if defined(__cplusplus)
#define ORB_DECLARE(_name) extern "C" const struct orb_metadata __orb_##_name
#else
#define ORB_DECLARE(_name) extern const struct orb_metadata __orb_##_name
#endif //__cplusplus


/**
 * Define (instantiate) the uORB metadata for a topic.
 *
 * The uORB metadata is used to help ensure that updates and
 * copies are accessing the right data.
 *
 * Note that there must be no more than one instance of this macro
 * for each topic.
 *
 * @param _name		The name of the topic.
 * @param _struct	The structure the topic provides.
 * @param _size_no_padding	Struct size w/o padding at the end
 * @param _fields	All fields in a semicolon separated list e.g: "float[3] position;bool armed"
 * @param _orb_id_enum	ORB ID enum e.g.: ORB_ID::vehicle_status
 */
#define ORB_DEFINE(_name, _struct, _size_no_padding, _fields, _orb_id_enum) \
    const struct orb_metadata __orb_##_name = {                             \
        #_name,                                                             \
        sizeof(_struct),                                                    \
        _size_no_padding,                                                   \
        _fields,                                                            \
        _orb_id_enum,                                                       \
    };                                                                      \
    struct hack


/**
 * ORB topic advertiser handle.
 *
 * Advertiser handles are global; once obtained they can be shared freely
 * and do not need to be closed or released.
 *
 * This permits publication from interrupt context and other contexts where
 * a file-descriptor-based handle would not otherwise be in scope for the
 * publisher.
 */
typedef void *orb_advertise_t;
typedef void *orb_subscribe_t;


struct rt_uorb_node
{
    rt_list_t                  list;
    const struct orb_metadata *meta;
    rt_uint8_t                 instance;
    rt_uint8_t                 queue_size;
    rt_uint32_t                generation;
    // rt_list_t          callbacks;
    rt_bool_t   advertised;
    rt_uint8_t  subscriber_count;
    rt_bool_t   data_valid;
    rt_uint8_t *data;
};

struct rt_uorb_subscribe
{
    const struct orb_metadata *meta;
    rt_uint8_t                 instance;
    rt_tick_t                  update_interval;

    struct rt_uorb_device *node;
    rt_uint32_t            generation;
    rt_tick_t              last_update;
    rt_bool_t              callback_registered;
};

struct rt_uorb_node *uorb_node_create(const struct orb_metadata *meta, const rt_uint8_t instance, rt_uint8_t queue_size);
rt_err_t uorb_node_delete(struct rt_uorb_node *node);
struct rt_uorb_node *uorb_node_find(const struct orb_metadata *meta, int instance);
int uorb_node_read(struct rt_uorb_node *node, void *data, int *generation);
int uorb_node_write(struct rt_uorb_node *node, void *data);
orb_subscribe_t orb_subscribe_multi(const struct orb_metadata *meta, unsigned instance);
int orb_check(orb_subscribe_t sub, rt_bool_t *updated);
int orb_copy(const struct orb_metadata *meta, int handle, void *buffer);
int orb_unsubscribe(int handle);
orb_advertise_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance,
                                          unsigned int queue_size);
int orb_publish(const struct orb_metadata *meta, orb_advertise_t handle, const void *data);
int orb_unadvertise(orb_advertise_t handle);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // _UORB_H_
