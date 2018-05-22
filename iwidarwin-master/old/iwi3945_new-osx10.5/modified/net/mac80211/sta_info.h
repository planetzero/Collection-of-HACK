/*
 * Copyright 2002-2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef STA_INFO_H
#define STA_INFO_H

#include <linux/list.h>
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/kref.h>
#include "ieee80211_key.h"

/* Stations flags (struct sta_info::flags) */
#define WLAN_STA_AUTH BIT(0)
#define WLAN_STA_ASSOC BIT(1)
#define WLAN_STA_PS BIT(2)
#define WLAN_STA_TIM BIT(3) /* TIM bit is on for PS stations */
#define WLAN_STA_PERM BIT(4) /* permanent; do not remove entry on expiration */
#define WLAN_STA_AUTHORIZED BIT(5) /* If 802.1X is used, this flag is
				    * controlling whether STA is authorized to
				    * send and receive non-IEEE 802.1X frames
				    */
#define WLAN_STA_SHORT_PREAMBLE BIT(7)
#define WLAN_STA_WME BIT(9)
#define WLAN_STA_HT  BIT(10)
#define WLAN_STA_WDS BIT(27)

#define STA_TID_NUM 16
#define ADDBA_RESP_INTERVAL HZ

enum ht_agg_state {
	HT_AGG_STATE_IDLE = 0,
	HT_AGG_STATE_START_BA,
	HT_AGG_STATE_ADDBA_REQUESTED,
	HT_AGG_STATE_OPERATIONAL,
	HT_AGG_STATE_INITIATOR_REQ_STOP_BA,
	HT_AGG_STATE_RECIPIENT_REQ_STOP_BA
};

struct tid_ht_agg_info_tx {
	enum ht_agg_state state;
	u8 dialog_token;
	u16 start_seq_num;
	struct timer_list addba_resp_timer; /* check if peer responds to addba request */
};

struct tid_ht_agg_info_rx {
	enum ht_agg_state state;
	u8 dialog_token;
	u16 start_seq_num;
	u16 head_seq_num;
	u16 buf_size;
	struct sk_buff** reordering_buf;
	u16 stored_mpdu_num;			   /* number of mpdus in reordering buffer */
	struct timer_list session_timer;   /* check if peer keeps Tx-ing on the TID */
	u16 timeout;
};

struct sta_ht_agg_info {
	u8 dialog_token_allocator;
	struct tid_ht_agg_info_tx tid_agg_info_tx[STA_TID_NUM];
	struct tid_ht_agg_info_rx tid_agg_info_rx[STA_TID_NUM];
	spinlock_t agg_data_lock_tx;
	spinlock_t agg_data_lock_rx;
};





struct sta_info {
	struct kref kref;
	struct list_head list;
	struct sta_info *hnext; /* next entry in hash table list */

	struct ieee80211_local *local;

	u8 addr[ETH_ALEN];
	u16 aid; /* STA's unique AID (1..2007), 0 = not yet assigned */
	u32 flags; /* WLAN_STA_ */

	struct sk_buff_head ps_tx_buf; /* buffer of TX frames for station in
					* power saving state */
	int pspoll; /* whether STA has send a PS Poll frame */
	struct sk_buff_head tx_filtered; /* buffer of TX frames that were
					  * already given to low-level driver,
					  * but were filtered */
	int clear_dst_mask;

	unsigned long rx_packets, tx_packets; /* number of RX/TX MSDUs */
	unsigned long rx_bytes, tx_bytes;
	unsigned long tx_retry_failed, tx_retry_count;
	unsigned long tx_filtered_count;

	unsigned int wep_weak_iv_count; /* number of RX frames with weak IV */

	unsigned long last_rx;
	u32 supp_rates; /* bitmap of supported rates in local->curr_rates */
	int txrate; /* index in local->curr_rates */
	int last_txrate; /* last rate used to send a frame to this STA */
	int last_nonerp_idx;

	struct net_device *dev; /* which net device is this station associated
				 * to */

	struct ieee80211_key *key;

	u32 tx_num_consecutive_failures;
	u32 tx_num_mpdu_ok;
	u32 tx_num_mpdu_fail;

	struct rate_control_ref *rate_ctrl;
	void *rate_ctrl_priv;

	/* last received seq/frag number from this STA (per RX queue) */
	__le16 last_seq_ctrl[NUM_RX_DATA_QUEUES];
	unsigned long num_duplicates; /* number of duplicate frames received
				       * from this STA */
	unsigned long tx_fragments; /* number of transmitted MPDUs */
	unsigned long rx_fragments; /* number of received MPDUs */
	unsigned long rx_dropped; /* number of dropped MPDUs from this STA */

	int accum_rssi; /* hi-precision running average (rssi * 16) */
	int accum_signal; /* hi-precision average (signal-quality * 16) */
	int accum_noise; /* hi-precision running average (noise * 16) */
	int last_rssi; /* average RSSI of recent frames from this STA */
	int last_signal; /* average sig-qual of recent frames from this STA */
	int last_noise; /* average noise of recent frames from this STA */
	int last_ack_rssi[3]; /* RSSI of last received ACKs from this STA */
	unsigned long last_ack;
	int channel_use;
	int channel_use_raw;

	u8 antenna_sel_tx;
	u8 antenna_sel_rx;


	int key_idx_compression; /* key table index for compression and TX
				  * filtering; used only if sta->key is not
				  * set */

#ifdef CONFIG_MAC80211_DEBUGFS
	int debugfs_registered;
#endif
	int assoc_ap; /* whether this is an AP that we are
		       * associated with as a client */
	int dls_sta; /* whether this stations is a DLS peer of us */

#define DLS_STATUS_OK		0
#define DLS_STATUS_NOLINK	1
	int dls_status;
	u32 dls_timeout;

#ifdef CONFIG_MAC80211_DEBUG_COUNTERS
	unsigned int wme_rx_queue[NUM_RX_DATA_QUEUES];
	unsigned int wme_tx_queue[NUM_RX_DATA_QUEUES];
#endif /* CONFIG_MAC80211_DEBUG_COUNTERS */

	int vlan_id;

	u16 listen_interval;
	struct sta_ht_agg_info ht_ba_mlme;
	int timer_to_tid[STA_TID_NUM]; /* converts expired addba response timer */
	int tx_queue_id[STA_TID_NUM];


#ifdef CONFIG_MAC80211_DEBUGFS
	struct sta_info_debugfsdentries {
		struct dentry *dir;
		struct dentry *flags;
		struct dentry *num_ps_buf_frames;
		struct dentry *last_ack_rssi;
		struct dentry *last_ack_ms;
		struct dentry *inactive_ms;
		struct dentry *last_seq_ctrl;
		struct dentry *agg_status;
#ifdef CONFIG_MAC80211_DEBUG_COUNTERS
		struct dentry *wme_rx_queue;
		struct dentry *wme_tx_queue;
#endif
	} debugfs;
#endif
};


/* Maximum number of concurrently registered stations */
#define MAX_STA_COUNT 2007

#define STA_HASH_SIZE 256
#define STA_HASH(sta) (sta[5])


/* Maximum number of frames to buffer per power saving station */
#define STA_MAX_TX_BUFFER 128

/* Minimum buffered frame expiry time. If STA uses listen interval that is
 * smaller than this value, the minimum value here is used instead. */
#define STA_TX_BUFFER_EXPIRE (10 * HZ)

/* How often station data is cleaned up (e.g., expiration of buffered frames)
 */
#define STA_INFO_CLEANUP_INTERVAL (10 * HZ)

struct sta_info * sta_info_get(struct ieee80211_local *local, u8 *addr);
int sta_info_min_txrate_get(struct ieee80211_local *local);
void sta_info_put(struct sta_info *sta);
struct sta_info * sta_info_add(struct ieee80211_local *local,
			       struct net_device *dev, u8 *addr, gfp_t gfp);
void sta_info_free(struct sta_info *sta, int locked);
void sta_info_init(struct ieee80211_local *local);
int sta_info_start(struct ieee80211_local *local);
void sta_info_stop(struct ieee80211_local *local);
void sta_info_remove_aid_ptr(struct sta_info *sta);
void sta_info_flush(struct ieee80211_local *local, struct net_device *dev);

#endif /* STA_INFO_H */
