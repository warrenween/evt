/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <eosio/chain/block_trace.hpp>
#include <eosio/chain/transaction.hpp>
#include <eosio/chain/transaction_metadata.hpp>
#include <eosio/chain/tokendb.hpp>
#include <fc/utility.hpp>
#include <sstream>
#include <algorithm>

namespace chainbase { class database; }

namespace eosio { namespace chain {

class chain_controller;

class apply_context {
public:
      apply_context(chain_controller& con, chainbase::database& db, evt::chain::tokendb& tokendb, const action& a, const transaction_metadata& trx_meta)

      :controller(con),
       db(db),
       act(a),
       mutable_controller(con),
       mutable_db(db),
       mutable_tokendb(tokendb),
       trx_meta(trx_meta)
       {}

      void exec();

    /**
       * @return true if account exists, false if it does not
       */
      bool is_account(const account_name& account)const;

      /**
       * Return true if the current action has already been scheduled to be
       * delivered to the specified account.
       */
      bool has_authorized(const domain_name& domain, const domain_key& key)const;

      vector<account_name> get_active_producers() const;

      const bytes&         get_packed_transaction();

      const chain_controller&       controller;
      const chainbase::database&    db;  ///< database where state is stored
      const action&                 act; ///< message being applied
      account_name                  receiver; ///< the code that is currently running
      bool                          privileged   = false;

      chain_controller&             mutable_controller;
      chainbase::database&          mutable_db;
      evt::chain::tokendb           mutable_tokendb;


      const transaction_metadata&   trx_meta;

      struct apply_results {
         vector<action_trace>          applied_actions;
      };

      apply_results results;

      template<typename T>
      void console_append(T val) {
         _pending_console_output << val;
      }

      template<typename T, typename ...Ts>
      void console_append(T val, Ts ...rest) {
         console_append(val);
         console_append(rest...);
      };

      inline void console_append_formatted(const string& fmt, const variant_object& vo) {
         console_append(fc::format_string(fmt, vo));
      }

      int get_action( uint32_t type, uint32_t index, char* buffer, size_t buffer_size )const;

   private:
      void append_results(apply_results &&other) {
         fc::move_append(results.applied_actions, move(other.applied_actions));
      }

      void exec_one();
      
      vector<account_name>                _notified; ///< keeps track of new accounts to be notifed of current message
      std::ostringstream                  _pending_console_output;

      bytes                               _cached_trx;
};

using apply_handler = std::function<void(apply_context&)>;

} } // namespace eosio::chain

FC_REFLECT(eosio::chain::apply_context::apply_results, (applied_actions))
