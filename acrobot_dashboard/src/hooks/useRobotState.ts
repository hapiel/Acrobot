import { GetStatusResponse } from '@/lib/AcroborAPI';
import { create } from 'zustand';

export type RobotStatus = {
  status: GetStatusResponse;
  setStatus: (status: GetStatusResponse) => void;
  setRightArm: (status: GetStatusResponse['rightArm']) => void;
  setLeftArm: (status: GetStatusResponse['leftArm']) => void;
  setRightLeg: (status: GetStatusResponse['rightLeg']) => void;
};

export const useRobotStatus = create<RobotStatus>((set) => ({
  status: {} as GetStatusResponse,
  setStatus: (status: GetStatusResponse) => set({ status }),
  setRightArm: (status: GetStatusResponse['rightArm']) => {
    set((state) => {
      return { status: { ...state.status, rightArm: status } };
    });
  },
  setLeftArm: (status: GetStatusResponse['leftArm']) => {
    set((state) => {
      return { status: { ...state.status, leftArm: status } };
    });
  },
  setRightLeg: (status: GetStatusResponse['rightLeg']) => {
    set((state) => {
      return { status: { ...state.status, rightLeg: status } };
    });
  },
  setLeftLeg: (status: GetStatusResponse['leftLeg']) => {
    set((state) => {
      return { status: { ...state.status, leftLeg: status } };
    });
  }
}));
