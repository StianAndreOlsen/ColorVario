#include "PageView.h"
#include "Color.h"

void Kystsoft::PageView::create(const Dali::Vector2& pageSize)
{
	// save page width since both scrollView.GetTargetSize() and
	// scrollView.GetCurrentSize() returns 0 before scrollView is shown
	pageWidth = pageSize.width;

	scrollView = Dali::Toolkit::ScrollView::New();
	scrollView.SetSize(pageSize);
	scrollView.SetParentOrigin(Dali::ParentOrigin::CENTER);
	scrollView.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	scrollView.SetPosition(0, 0);
	scrollView.SetBackgroundColor(Color::window());
//	scrollView.SetOvershootEffectColor(Color::button()); // TODO: Discuss with Kyrre!

	rulerX = new Dali::Toolkit::FixedRuler(pageSize.width);
	scrollView.SetRulerX(rulerX);
	Dali::Toolkit::RulerPtr rulerY = new Dali::Toolkit::DefaultRuler();
	rulerY->Disable();
	scrollView.SetRulerY(rulerY);
	scrollView.SetScrollingDirection(Dali::PanGestureDetector::DIRECTION_HORIZONTAL);

	scrollView.OnStageSignal().Connect(this, &PageView::onStageEntered);
	scrollView.KeyEventSignal().Connect(this, &PageView::onKeyEvent);
	scrollView.ScrollCompletedSignal().Connect(this, &PageView::onScrollCompleted);
}

void Kystsoft::PageView::addPage(Dali::Actor page)
{
	scrollView.Add(page);
	rulerX->SetDomain(Dali::Toolkit::RulerDomain(0, pageCount() * pageWidth));
}

void Kystsoft::PageView::onWheelEvent(const Dali::WheelEvent& event)
{
	int page = targetPage;
	if (event.z > 0)
		++page;
	else if (event.z < 0)
		--page;
	scrollTo(page);
}

void Kystsoft::PageView::scrollTo(int pageIndex)
{
	if (pageIndex < 0)
	{
		targetPage = 0;
		scrollView.ScrollTo(Dali::Vector2(-pageWidth, 0));
	}
	else if (pageIndex >= pageCount())
	{
		targetPage = pageCount() - 1;
		scrollView.ScrollTo(Dali::Vector2(pageCount() * pageWidth, 0));
	}
	else
	{
		targetPage = pageIndex;
		scrollView.ScrollTo(static_cast<unsigned int>(pageIndex));
	}
}

bool Kystsoft::PageView::onKeyEvent(Dali::Toolkit::Control /*control*/, const Dali::KeyEvent& event)
{
	if (event.state == Dali::KeyEvent::Up)
	{
		if (Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) ||
		    Dali::IsKey(event, Dali::DALI_KEY_BACK))
		{
			goBackSignl.emit();
			return true;
		}
	}
	return false;
}

void Kystsoft::PageView::onScrollCompleted(const Dali::Vector2& /*currentScrollPosition*/)
{
	int pageIndex = static_cast<int>(scrollView.GetCurrentPage());
	if (currentPage != pageIndex)
	{
		currentPage = targetPage = pageIndex;
		currentPageChangedSignl.emit(pageIndex);
	}
}
