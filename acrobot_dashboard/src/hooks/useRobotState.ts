import { create } from 'zustand';

export type LimbStatus = {
  target: number;
  position: number;
  velocity: number;
  torque: number;
  temperature: number;
  isCalibrating: boolean;
  isOnline: boolean;
};

export type RobotStatus = {
  leftLeg: LimbStatus;
  rightLeg: LimbStatus;
  leftArm: LimbStatus;
  rightArm: LimbStatus;
};

export type RobotStatusStore = {
  status: RobotStatus;
  setStatus: (status: RobotStatus) => void;
  setRightArm: (status: RobotStatus['rightArm']) => void;
  setLeftArm: (status: RobotStatus['leftArm']) => void;
  setRightLeg: (status: RobotStatus['rightLeg']) => void;
  setLeftLeg: (status: RobotStatus['leftLeg']) => void;
};

export const useRobotStatus = create<RobotStatusStore>((set) => ({
  setStatus: (status: RobotStatus) => set({ status }),
  setRightArm: (status: RobotStatus['rightArm']) => {
    set((state) => {
      return { status: { ...state.status, rightArm: status } };
    });
  },
  setLeftArm: (status: RobotStatus['leftArm']) => {
    set((state) => {
      return { status: { ...state.status, leftArm: status } };
    });
  },
  setRightLeg: (status: RobotStatus['rightLeg']) => {
    set((state) => {
      return { status: { ...state.status, rightLeg: status } };
    });
  },
  setLeftLeg: (status: RobotStatus['leftLeg']) => {
    set((state) => {
      return { status: { ...state.status, leftLeg: status } };
    });
  },
  status: {
    leftArm: {
      target: 0,
      position: 0,
      velocity: 0,
      torque: 0,
      temperature: 0,
      isCalibrating: true,
      isOnline: false
    },
    rightArm: {
      target: 0,
      position: 0,
      velocity: 0,
      torque: 0,
      temperature: 0,
      isCalibrating: true,
      isOnline: false
    },
    leftLeg: {
      target: 0,
      position: 0,
      velocity: 0,
      torque: 0,
      temperature: 0,
      isCalibrating: true,
      isOnline: false
    },
    rightLeg: {
      target: 0,
      position: 0,
      velocity: 0,
      torque: 0,
      temperature: 0,
      isCalibrating: true,
      isOnline: false
    }
  } satisfies RobotStatus
}));
