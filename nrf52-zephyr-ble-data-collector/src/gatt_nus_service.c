
#include <zephyr/types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <data/json.h>

#include "gatt_nus_service.h"
#include "state.h"

static u8_t notify_nus;
//static u8_t nus_rx[64];

static void nus_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                u16_t value)
{
  notify_nus = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

/*static ssize_t on_read_rx(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, u16_t len, u16_t offset)
{
  return bt_gatt_attr_read(conn, attr, buf, len, offset, &nus_rx, sizeof(nus_rx));
}

static ssize_t on_write_rx(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr,
                           const void *buf, u16_t len, u16_t offset,
                           u8_t flags);*/

BT_GATT_SERVICE_DEFINE(nus_cvs,
                       BT_GATT_PRIMARY_SERVICE(BT_NUS_SERVICE_UUID),

                       /*BT_GATT_CHARACTERISTIC(BT_RX_CHARACTERISTIC_UUID,
                                              BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                                              BT_GATT_PERM_WRITE, on_read_rx, on_write_rx, &nus_rx),*/

                       BT_GATT_CHARACTERISTIC(BT_TX_CHARACTERISTIC_UUID,
                                              BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),

                       BT_GATT_CCC(nus_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

/*static ssize_t on_write_rx(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr,
                           const void *buf, u16_t len, u16_t offset,
                           u8_t flags)
{
  u8_t *value = attr->user_data;

  if (offset + len > sizeof(nus_rx))
  {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  memcpy(value + offset, buf, len);

  if (len == 0)
  {
    return len;
  }

  printk("Received cmd (%s)\n", log_strdup(value));

  return len;
}*/

s8_t initNusDone = 0;

void gatt_nus_service_init(void)
{
  initNusDone = 1;
}

void gatt_nus_service_data_notify(struct bt_conn *conn)
{
  if (!initNusDone)
  {
    return;
  }

  if (!notify_nus)
  {
    return;
  }

  int ret = 1;
  //static char buf[40];
  struct ble_reading br = get_ble_reading();
  //ret = json_obj_encode_buf(status_msg_descr, ARRAY_SIZE(status_msg_descr), &br, buf, sizeof(buf));
  //ret = sprintf(buf, "%d,%d,%d,%d,%d,%d", br.x1, br.x2, br.y1, br.y2, br.z1, br.z2);
  if (ret >= 0)
  {
    //bt_gatt_notify(conn, &nus_cvs.attrs[2], buf, strlen(buf));
    bt_gatt_notify(conn, &nus_cvs.attrs[2], &br, sizeof(struct ble_reading));
    //printk("Sending new data (%s)\n", buf);
  }
}
