/*
// 3rd interrupt ISR - 20200908

// 2nd priority & interrupt - 20200908

// 1st ipc
Server pseudo-code:
   • create a channel ( ChannelCreate() )
   • wait for a message ( MsgReceive() )
   – perform processing
   – reply ( MsgReply() )
Client pseudo-code:
   • attach to a channel ( ConnectAttach() )
   • send message ( MsgSend() )
*/

There are:

ipc_test/

ipc_server/

ipc_client/


