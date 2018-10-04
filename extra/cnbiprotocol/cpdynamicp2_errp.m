% 2011-04-30  Michele Tavella <michele.tavella@epfl.ch>
% function dynamicp2_errp(S, Pe, Tmax)
%
% Finds all the possible combinations of trial numbers for E = [0 1 2 3 4]
% with S squares that lead to a Pe (i.e. 0.2) balanced dataset.
%
% Tmax is the maximum number of wanted trials.
%
% Example: cpdynamicp2_errp(6, 0.20, 240)
function dynamicp2_errp(S, Pe, Tmax)

if(nargin < 1)
	S = 6;
end

E = [0:1:4];
T = 6 + E*2;
C = T - E;

N = [1:1:10];

for n0 = N
	for n1 = N
		for n2 = N
			for n3 = N
				for n4 = N
					v = [n0 n1 n2 n3 n4];
					eE = sum(v.*E);
					eT = sum(v.*T);
					eC = sum(v.*C);
					tPe = eE/eT;
					if(tPe == Pe & eT == Tmax)
						fprintf(1, 'T=%3d, E=%3d, C=%3d <--> %2d %2d %2d %2d %2d\n', ...
							eT, eE, eC, v(1), v(2), v(3), v(4), v(5));
					end
				end
			end
		end
	end
end
