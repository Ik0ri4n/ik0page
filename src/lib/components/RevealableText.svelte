<script lang="ts">
	import { ClipboardIcon, EyeIcon, EyeOffIcon } from '@rgossiaux/svelte-heroicons/outline';
	import { addToast } from './Toaster.svelte';
	import Button from './Button.svelte';

	export let description: string;
	export let value: string = '';

	let hidden: boolean = true;

	function handleToggle(e: MouseEvent) {
		e.preventDefault();

		hidden = !hidden;
	}
	function handleCopy(e: MouseEvent) {
		e.preventDefault();

		navigator.clipboard.writeText(value);

		addToast({ data: { title: `Copied ${description}.` } });
	}
</script>

<div class="flex w-full items-center gap-2">
	<div
		class="flex flex-1 h-10 items-center overflow-x-scroll whitespace-nowrap rounded border border-gray-300 px-1 dark:border-gray-700"
	>
		{#if hidden}
			<span
				class="select-none rounded-sm bg-gray-900 px-1 font-mono text-transparent"
				title="Click to reveal">{'_'.repeat(value.length)}</span
			>
		{:else}
			<span class="px-1 font-mono">
				{value}
			</span>
		{/if}
	</div>

	<Button size="h-10" ariaLabel={hidden ? 'Show' : 'Hide'} on:click={handleToggle}>
		{#if hidden}
			<EyeOffIcon class="h-6 w-6" />
		{:else}
			<EyeIcon class="h-6 w-6" />
		{/if}
	</Button>
	<Button size="h-10" ariaLabel="Copy" on:click={handleCopy}>
		<ClipboardIcon class="h-6 w-6" />
	</Button>
</div>
