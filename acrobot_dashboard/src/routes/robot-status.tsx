import { Limb, LimbName } from '@/components/limb/Limb';
import { useRobotStatus } from '@/hooks/useRobotState';
import { cn } from '@/lib/utils';
import useWebSocket, { ReadyState } from 'react-use-websocket';

export function RobotStatus() {
  const {
    status: { rightArm, rightLeg, leftArm, leftLeg },
    setStatus
  } = useRobotStatus();

  const { readyState } = useWebSocket('ws://acrobot.local:3000/ws', {
    shouldReconnect: () => true,
    onMessage: (message) => {
      const data = message.data;
      if (typeof data !== 'string') {
        return;
      }

      if (data.toLowerCase().startsWith('hello client')) {
        return;
      }

      try {
        const status = JSON.parse(data);
        setStatus(status);
      } catch (error) {
        console.error(error);
      }
    }
  });

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-2">
      <div className="flex flex-col gap-2"></div>
      <div className="relative cursor-pointer rounded-lg rounded-tr-none bg-gray-100/10 px-4 py-2 transition-all">
        <div
          className={cn(
            'absolute -right-0.5 -top-0.5 size-2 rounded-full',
            readyState === ReadyState.OPEN ? 'bg-green-300' : 'bg-red-300'
          )}
        ></div>
        <div
          className={cn(
            'absolute -right-0.5 -top-0.5 size-2 animate-ping rounded-full',
            readyState === ReadyState.OPEN ? 'bg-green-300' : 'bg-red-300'
          )}
        ></div>
        <h2 className="text-center">Socket status</h2>
        <p className="text-center">{ReadyState[readyState]}</p>
      </div>
      <div className="grid w-full grid-cols-2 grid-rows-2 gap-4 p-2">
        <Limb status={rightArm}>
          <LimbName>Right arm</LimbName>
        </Limb>
        <Limb status={leftArm}>
          <LimbName>Left arm</LimbName>
        </Limb>
        <Limb status={rightLeg}>
          <LimbName>Right leg</LimbName>
        </Limb>
        <Limb status={leftLeg}>
          <LimbName>Left leg</LimbName>
        </Limb>
      </div>
    </main>
  );
}
