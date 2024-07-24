import { ReadyState } from 'react-use-websocket';
import { create } from 'zustand';

export type WsStatus = {
  status: ReadyState;
  isConnected: () => boolean;
  set: (status: ReadyState) => void;
};

export const useWsStatus = create<WsStatus>((set, get) => ({
  status: ReadyState.CLOSED,
  isConnected: () => {
    console.log(get().status);
    return get().status === ReadyState.OPEN;
  },
  set: (status: ReadyState) => set({ status })
}));
