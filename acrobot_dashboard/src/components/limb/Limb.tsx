import { LimbStatus } from '@/lib/AcroborAPI';
import { cn } from '@/lib/utils';

export type LimbProps = {
  status?: LimbStatus;
  children: React.ReactNode;
};
export function Limb({ status, children }: LimbProps) {
  return (
    <div
      className={cn(
        'rounded-lg border transition-all',
        status?.isOnline ? 'border-green-600 dark:border-green-600' : 'border-red-600 dark:border-red-600'
      )}
    >
      <div className="flex flex-col">
        {children}
        <div className="rounded-b-lg *:px-2">
          <div className="even:bg-gray-600/50">
            <h2 className="font-bold">Position</h2>
            {status?.isOnline ? <p>{status?.position ?? '-'}</p> : <p>-</p>}
          </div>
          <div className="even:bg-gray-600/50">
            <h2 className="font-bold">Velocity</h2>
            {status?.isOnline ? <p>{status?.velocity ?? '-'}</p> : <p>-</p>}
          </div>
          <div className="even:bg-gray-600/50">
            <h2 className="font-bold">Torque</h2>
            {status?.isOnline ? <p>{status?.torque ?? '-'}</p> : <p>-</p>}
          </div>
          <div className="even:bg-gray-600/50">
            <h2 className="font-bold">Temperature</h2>
            {status?.isOnline ? <p>{status?.temperature ?? '-'}</p> : <p>-</p>}
          </div>
          <div className="even:bg-gray-600/50">
            <h2 className="font-bold">Status</h2>
            <p className={cn(status?.isOnline ? 'text-green-600' : 'text-red-600')}>
              {status?.isOnline ? 'Online' : 'Offline'}
            </p>
          </div>
        </div>
      </div>
    </div>
  );
}
export type LimbNameProps = {
  children: React.ReactNode;
};

export function LimbName({ children }: LimbNameProps) {
  return (
    <h1 className="rounded-t-lg bg-gray-800/60 p-2 text-xl font-bold uppercase dark:bg-gray-200/30">{children}</h1>
  );
}
