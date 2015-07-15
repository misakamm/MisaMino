function Start()
	SetThinkDepth(0) ;
-- 	SetHoldMinDiff(100) ;
-- 	SetThinkDelayFrame(10) ;
-- 	SetMoveDelayFrame(10) ;
-- 	
	SetHoldMinDiff(1000) ;
	SetThinkDelayFrame(0) ;
	SetMoveDelayFrame(0) ;
end


function End()
end

function PrepareThink()
	local height = GetBlockHighest() - (GetHeight() - GetBlockLastHeight()) ;
	
end

function EvalField(depth)

	--int x,y,i,full,empty,btype,well,blocked,lblock,rblock;
	-- Intermediate Results
	local HorizontalTransitions = 0;--가로로 0 1 0 1 반복정도
	local FilledLines = 0;
	local HighestBlock = -1;
	local VerticalTransitions = 0;--위로 0 1 0 1 반복정도
	local BlockedCells = 0;--구멍의 위를 막은정도
	local Wells = 0; --양쪽에 블럭이 있을때 블럭의 깊이 파인정도 
	local StartScore = 0 ;
	local BlockedWells = 0 ; --구멍을 막은 블럭의 높이
	
	local efield = {} ;
	local x,y,i,full,empty,btype,well,blocked,lblock,rblock;
	local width, height, startHeight ;
	
	width = GetWidth() ;  
	--height = GetHeight() ;
	height = GetBlockLastHeight() ;
	startHeight = GetHeight()-GetBlockHighest() ;
	
	HighestBlock = height ;
	
	--Debug(string.format('HighestBlock(%d)',HighestBlock)) ;

	-- Copy to eField, Calculate HorizontalTransitions, FilledLines and HighestBlock
	y=height-1 ;
	i=y ;
	while(y>=0) do
		btype=1; full=1; empty=1;
		x=width-1 ;
		while (x>=0) do
			if(0 == GetField(x, y)) then
				efield[x+i*width]=0;
			else
				efield[x+i*width]=1 ;
				empty = 0 ;
			end

			if(btype ~= efield[x+i*width]) then
				btype = efield[x+i*width];
				full=0;
				HorizontalTransitions = HorizontalTransitions + 1;
			end
			x = x-1 ;
		end
		
		if(0 == full) then
			i = i-1;
		else
			FilledLines = FilledLines + 1 ;
		end
		
		if(0 == btype) then
			HorizontalTransitions = HorizontalTransitions + 1;
		end
		
		if(i < startHeight and empty == 1) then
			HighestBlock = height-(i+2);
			break ;
		end
		
		y = y-1 ;
	end
	
	
	x=0 ;
	while(x<width) do
		btype=0; blocked=0; blockedHeight = 0 ;
		y = height-HighestBlock ;
		while(y<height) do
			
			if(btype~=efield[x+y*width]) then
				if(blocked == 0) then
					blocked=1;
					blockedHeight = y ;
				end
				btype=efield[x+y*width];
				VerticalTransitions= VerticalTransitions+1;
			end
			
			-- 위에가 막히고 현재 자리가 비어있다면 BlockedCells증가
			if(blocked == 1 and 0 == efield[x+y*width]) then
				BlockedCells = BlockedCells + 1 ;
				BlockedWells = BlockedWells + (y - blockedHeight) ;
			end
			
			if(x==0) then
				lblock=1;
			else
				lblock = efield[(x-1)+y*width];
			end
			
			if(x==width-1) then
				rblock=1;
			else
				rblock = efield[(x+1)+y*width];
			end
			
			
			if(lblock == 1 and rblock == 1) then --좌우에 블럭이 있을때
				i=y ;
				while(i<height) do
					if(efield[x+i*width]~=0) then --중간이 비어있다면
						break ;
					end
					Wells=Wells+1; --well을 증가
					--Debug(string.format('wells line : %d', i)) ;
					i = i+1 ;
				end
			end
			y = y+1 ;
		end
		if(0 == btype) then
			VerticalTransitions=VerticalTransitions+1;
		end
		x=x+1 ;
	end
	
	--HorizontalTransitions = HorizontalTransitions - HorizontalTransitions % 5 ;
	--VerticalTransitions = VerticalTransitions - VerticalTransitions % 5 ;
	--BlockedCells = BlockedCells - BlockedCells % 5 ;
	--Wells = Wells - Wells % 5 ;
	--BlockedWells = BlockedWells - BlockedWells % 5 ;
	--HighestBlock = HighestBlock - HighestBlock % 5 ;
	
	local FinalScore = 0 ;
	
	FinalScore = 
		StartScore
		+(-10 * HighestBlock)
	--	+(-10 * HorizontalTransitions)
		+(-100 * VerticalTransitions)
	--	+(-10 * BlockedCells)
	--	+(-9 * Wells)
		+(1 * FilledLines)
	--	+(-10 * BlockedWells)
		 ;

	
	
	
	--if(curThinkState_ == THINK_STATE_TETRIS_BUILD) then
	--	if(FilledLines > 0 and FilledLines < 3) then
			--Debug(string.format('Score : %d, FillLine : %d, height : %d, blockCells : %d', FinalScore, FilledLines, HighestBlock, BlockedCells)) ;
	--	end
	--end
	
-- 	Debug(string.format('Score(%d), StartScore(%d), HighestBlock(%d), HorizontalTransitions(%d), VerticalTransitions(%d), BlockedCells(%d), Wells(%d), FilledLine(%d), BlockedWells(%d)',
-- 		FinalScore,
-- 		StartScore,
-- 		HighestBlock,
-- 		HorizontalTransitions,
-- 		VerticalTransitions,
-- 		BlockedCells,
-- 		Wells,
-- 		FilledLines,
-- 		BlockedWells
-- 		)) ;
		
		
	return FinalScore;
	
end
